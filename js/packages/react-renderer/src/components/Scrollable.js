import React, { useState, useRef, useCallback, useEffect } from 'react'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import VelocityTracker from '@advk/common/src/gestures/VelocityTracker.js'
import { Responder, Scroll } from '../nativeComponents'
import KeyCode from './KeyCode.js'

const KeyAction = {
	PRESS: 0,
	RELEASE: 1,
	REPEAT: 2
}

const KEYBOARD_SCROLL_DISTANCE = 10

const overscroll = overscrollValue => Math.pow(overscrollValue, 0.8)

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

const onDragMove = (ctx, event) => {
	const maxScrollTop = getMaxScrollTop(ctx)
	let nextScrollTop = ctx.initialScrollTop - event.deltaTop
	if (nextScrollTop < 0) {
		nextScrollTop = -overscroll(-nextScrollTop)
	} else if (nextScrollTop > maxScrollTop) {
		nextScrollTop = maxScrollTop + overscroll(nextScrollTop - maxScrollTop)
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

const onKeyEvent = (ctx, event) => {
	if (event.action === KeyAction.PRESS || event.action === KeyAction.REPEAT) {
		if (event.key === KeyCode.UP || event.key == KeyCode.DOWN) {
			let nextScrollTop =
				ctx.scrollTopValue.__getValue() +
				(event.key === KeyCode.UP ? 1 : -1) * KEYBOARD_SCROLL_DISTANCE
			nextScrollTop = Math.max(nextScrollTop, 0)
			Animated.spring(ctx.scrollTopValue, {
				toValue: nextScrollTop,
				overshootClamping: true
			}).start()
		}
	}
}

const Scrollable = ({ children }) => {
	const ref = useRef()
	const ctx = useRef({
		elemRef: ref,
		initialScrollTop: 0,
		scrollTopValue: new Animated.Value(0),
		velocityTracker: new VelocityTracker()
	})
	useEffect(() =>
		ctx.current.scrollTopValue.addListener(({ value }) => {
			ref.current.scrollTop = Math.round(value)
			ctx.current.velocityTracker.addPoint(Date.now(), value)
		})
	)
	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: () => {
						ctx.current.removeKeyHandler = ref.current.document.addKeyHandler(
							event => onKeyEvent(ctx.current, event)
						)
					},
					onHoverEnd: () => {
						if (ctx.current.removeKeyHandler)
							ctx.current.removeKeyHandler()
						ctx.current.removeKeyHandler = undefined
					}
				}),
				drag: new DragRecognizer({
					document: () => ref.current.document,
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
			<Scroll ref={ref}>{children}</Scroll>
		</Responder>
	)
}

export default Scrollable
