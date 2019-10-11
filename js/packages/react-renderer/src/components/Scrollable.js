import React, { useState, useRef, useCallback, useEffect } from 'react'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import VelocityTracker from '@advk/common/src/gestures/VelocityTracker.js'
import { KeyCode, KeyAction } from '@advk/common/src/events/KeyEvent.js'
import { Responder, Scroll } from '../nativeComponents'
import useLastProps from '../useLastProps.js'

const OverscrollResistance = {
	none: value => value,
	full: value => 0,
	diminishing: value => Math.pow(value, 0.8)
}

// Checks if element has overscroll and animates scroll position to normal
const animateOverscroll = ctx => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	const maxScrollTop = getMaxScrollTop(ctx)
	let toValue = undefined
	if (scrollTop < 0) toValue = 0
	else if (scrollTop > maxScrollTop) toValue = maxScrollTop
	if (toValue === undefined) return false
	Animated.spring(ctx.scrollTopValue, {
		toValue,
		bounciness: 6,
		// Velocity is based on seconds instead of milliseconds
		velocity: ctx.velocityTracker.getVelocity() * 1000
	}).start()
	return true
}

const getMaxScrollTop = ctx => {
	const elem = ctx.elemRef.current
	return elem.scrollHeight - elem.height
}

const clampScrollTop = (ctx, scrollTop) =>
	Math.min(Math.max(scrollTop, 0), getMaxScrollTop(ctx))

const onHoverStart = ctx => {
	const document = ctx.elemRef.current.document
	ctx.removeKeyHandler = document.addKeyHandler(event =>
		onKeyEvent(ctx, event)
	)
	ctx.removeScrollHandler = document.addScrollHandler(event =>
		onScrollEvent(ctx, event)
	)
}

const animateScrollTop = (ctx, nextScrollTop) => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	if (nextScrollTop === scrollTop) return
	Animated.spring(ctx.scrollTopValue, {
		toValue: nextScrollTop,
		overshootClamping: true
	}).start()
}

const onKeyEvent = (ctx, event) => {
	if (
		(event.action === KeyAction.PRESS ||
			event.action === KeyAction.REPEAT) &&
		(event.key === KeyCode.UP || event.key == KeyCode.DOWN)
	) {
		const scrollTop = ctx.scrollTopValue.__getValue()
		const delta =
			(event.key === KeyCode.UP ? 1 : -1) *
			ctx.getProps().keyboardScrollSpeed
		animateScrollTop(ctx, clampScrollTop(ctx, scrollTop + delta))
	}
}

const onScrollEvent = (ctx, event) => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	const nextScrollTop = clampScrollTop(
		ctx,
		scrollTop + -1 * event.top * ctx.getProps().mousewheelScrollSpeed
	)
	animateScrollTop(ctx, nextScrollTop)
}

const onDragMove = (ctx, event) => {
	const maxScrollTop = getMaxScrollTop(ctx)
	let nextScrollTop = ctx.initialScrollTop - event.deltaTop
	const overscrollResistance = ctx.getProps().overscrollResistance
	if (nextScrollTop < 0) {
		nextScrollTop = -overscrollResistance(-nextScrollTop)
	} else if (nextScrollTop > maxScrollTop) {
		nextScrollTop =
			maxScrollTop + overscrollResistance(nextScrollTop - maxScrollTop)
	}
	ctx.scrollTopValue.setValue(nextScrollTop)
}

const onDragEnd = (ctx, event) => {
	if (animateOverscroll(ctx)) return

	ctx.decayListener = ctx.scrollTopValue.addListener(() =>
		animateOverscroll(ctx)
	)
	Animated.decay(ctx.scrollTopValue, { velocity: -event.velocity }).start(
		({ finished }) => {
			ctx.scrollTopValue.removeListener(ctx.decayListener)
		}
	)
}

const Scrollable = props => {
	const getProps = useLastProps(props)
	const elemRef = useRef()
	const ctx = useRef({
		getProps,
		elemRef,
		initialScrollTop: 0,
		scrollTopValue: new Animated.Value(0),
		velocityTracker: new VelocityTracker()
	})
	useEffect(() =>
		ctx.current.scrollTopValue.addListener(({ value }) => {
			elemRef.current.scrollTop = Math.round(value)
			ctx.current.velocityTracker.addPoint(Date.now(), value)
		})
	)
	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: onHoverStart.bind(null, ctx.current),
					onHoverEnd: () => {
						if (ctx.current.removeKeyHandler)
							ctx.current.removeKeyHandler()
						ctx.current.removeKeyHandler = undefined
					}
				}),
				drag: new DragRecognizer({
					document: () => elemRef.current.document,
					onDragStart: event => {
						ctx.current.initialScrollTop = ctx.current.scrollTopValue.__getValue()
					},
					onDragMove: event => onDragMove(ctx.current, event),
					onDragEnd: event => onDragEnd(ctx.current, event)
				})
			})
	)
	return (
		<Responder handler={useCallback(recognizer.handler.bind(recognizer))}>
			<Scroll ref={elemRef}>{props.children}</Scroll>
		</Responder>
	)
}

Scrollable.defaultProps = {
	overscrollResistance: OverscrollResistance.diminishing,
	keyboardScrollSpeed: 10,
	mousewheelScrollSpeed: 100
}

export { OverscrollResistance }
export default Scrollable
