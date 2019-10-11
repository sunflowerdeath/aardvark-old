import React, { useState, useRef, useCallback, useEffect } from 'react'
import PropTypes from 'prop-types'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import RawRecognizer from '@advk/common/src/gestures/RawRecognizer.js'
import VelocityTracker from '@advk/common/src/gestures/VelocityTracker.js'
import { KeyCode, KeyAction } from '@advk/common/src/events/KeyEvent.js'
import {
	PointerTool,
	PointerAction
} from '@advk/common/src/events/PointerEvent.js'
import { Responder, Scroll } from '../nativeComponents'
import useLastProps from '../useLastProps.js'

const OverscrollResistance = {
	none: value => value,
	full: value => 0,
	diminishing: value => Math.pow(value, 0.8)
}

const removeHandler = (object, key) => {
	if (object[key]) {
		object[key]()
		object[key] = undefined
	}
}

const getMaxScrollTop = ctx => {
	const elem = ctx.elemRef.current
	return elem.scrollHeight - elem.height
}

const clampScrollTop = (ctx, scrollTop) =>
	Math.min(Math.max(scrollTop, 0), getMaxScrollTop(ctx))

const animateOverscroll = ctx => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	const toValue = clampScrollTop(ctx, scrollTop)
	if (toValue === scrollTop) return false
	Animated.spring(ctx.scrollTopValue, {
		toValue,
		bounciness: ctx.getProps().dragOverscrollBounciness,
		// Velocity is based on seconds instead of milliseconds
		velocity: ctx.velocityTracker.getVelocity() * 1000
	}).start()
	return true
}

const animateScroll = (ctx, nextScrollTop) => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	if (nextScrollTop === scrollTop) return
	Animated.spring(ctx.scrollTopValue, {
		toValue: nextScrollTop,
		overshootClamping: true
        // TODO configure speed
	}).start()
}

// Keyboard

// which Scrollable currently handles key events
let currentKeyHandler

const makeCurrentKeyHandler = ctx => {
	if (currentKeyHandler) currentKeyHandler.removeKeyHandler()
	const document = ctx.elemRef.current.document
	ctx.removeKeyHandler = document.addKeyHandler(onKeyEvent.bind(null, ctx))
	currentKeyHandler = ctx
}

const onRawEvent = (ctx, event) => {
	if (
		event.tool === PointerTool.MOUSE &&
		event.action === PointerAction.BUTTON_PRESS
	) {
		// TODO when multiple nested - should register on arena
		// and first one should win
		makeCurrentKeyHandler(ctx)
	}
}

const onKeyEvent = (ctx, event) => {
	if (event.action === KeyAction.PRESS || event.action === KeyAction.REPEAT) {
		if (event.key === KeyCode.UP || event.key == KeyCode.DOWN) {
			const scrollTop = ctx.scrollTopValue.__getValue()
			const delta =
				(event.key === KeyCode.UP ? -1 : 1) *
				ctx.getProps().keyboardScrollSpeed
			animateScroll(ctx, clampScrollTop(ctx, scrollTop + delta))
		} else if (event.key === KeyCode.HOME) {
			animateScroll(ctx, 0)
		} else if (event.key === KeyCode.END) {
			animateScroll(ctx, getMaxScrollTop(ctx))
		} else if (
			event.key === KeyCode.PAGE_UP ||
			event.key === KeyCode.PAGE_DOWN
		) {
			const scrollTop = ctx.scrollTopValue.__getValue()
			const delta =
				(event.key === KeyCode.PAGE_UP ? -1 : 1) *
				ctx.elemRef.current.height
			animateScroll(ctx, clampScrollTop(ctx, scrollTop + delta))
		}
	}
}

// Mouse wheel
const onHoverStart = ctx => {
	const document = ctx.elemRef.current.document
	ctx.removeScrollHandler = document.addScrollHandler(event =>
		onScrollEvent(ctx, event)
	)
}

const onHoverEnd = ctx => removeHandler(ctx, 'removeScrollHandler')

const onScrollEvent = (ctx, event) => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	const nextScrollTop = clampScrollTop(
		ctx,
		scrollTop + -1 * event.top * ctx.getProps().mousewheelScrollSpeed
	)
	animateScroll(ctx, nextScrollTop)
}

// Touch
const onDragStart = (ctx, event) =>
	(ctx.initialScrollTop = ctx.scrollTopValue.__getValue())

const onDragMove = (ctx, event) => {
	const maxScrollTop = getMaxScrollTop(ctx)
	let nextScrollTop = ctx.initialScrollTop - event.deltaTop
	const overscrollResistance = ctx.getProps().dragOverscrollResistance
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
	Animated.decay(ctx.scrollTopValue, {
		velocity: -event.velocity,
		deceleration: ctx.getProps().dragDecayDeceleration
	}).start(({ finished }) => {
		ctx.scrollTopValue.removeListener(ctx.decayListener)
	})
}

const unmount = ctx => {
	removeHandler(ctx, 'removeScrollHandler')
	removeHandler(ctx, 'removeKeyHandler')
	if (currentKeyHandler === ctx) {
		currentKeyHandler = undefined
		removeHandler(ctx, 'removeKeyHandler')
	}
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
	useEffect(() => unmount.bind(ctx))
	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: onHoverStart.bind(null, ctx.current),
					onHoverEnd: onHoverEnd.bind(null, ctx.current)
				}),
				drag: new DragRecognizer({
					document: () => elemRef.current.document,
					onDragStart: onDragStart.bind(null, ctx.current),
					onDragMove: onDragMove.bind(null, ctx.current),
					onDragEnd: onDragEnd.bind(null, ctx.current)
				}),
				raw: new RawRecognizer({
					handler: onRawEvent.bind(null, ctx.current)
				})
			})
	)
	return (
		<Responder handler={useCallback(recognizer.handler.bind(recognizer))}>
			<Scroll ref={elemRef}>{props.children}</Scroll>
		</Responder>
	)
}

Scrollable.propTypes = {
	dragOverscrollBounciness: PropTypes.number.isRequired,
	dragOverscrollResistance: PropTypes.func.isRequired,
	dragDecayDeceleration: PropTypes.number.isRequired,
	keyboardScrollSpeed: PropTypes.number.isRequired,
	mousewheelScrollSpeed: PropTypes.number.isRequired
}

Scrollable.defaultProps = {
	dragOverscrollBounciness: 6,
	dragOverscrollResistance: OverscrollResistance.diminishing,
	dragDecayDeceleration: 0.998,
	keyboardScrollSpeed: 50,
	mousewheelScrollSpeed: 100
}

export { OverscrollResistance }
export default Scrollable
