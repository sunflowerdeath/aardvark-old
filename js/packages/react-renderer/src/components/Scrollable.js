import React, { useState, useRef, useCallback, useEffect } from 'react'
import PropTypes from 'prop-types'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import RawRecognizer from '@advk/common/src/gestures/RawRecognizer.js'
import VelocityTracker from '@advk/common/src/gestures/VelocityTracker.js'
import { KeyCode, KeyAction } from '@advk/common/src/events/KeyEvent.js'
import { isMouseButtonPress } from '@advk/common/src/events/PointerEvent.js'
import { Responder, Scroll } from '../nativeComponents'
import useContext from '../hooks/useContext.js'

const OverscrollResistance = {
	none: value => value,
	full: value => 0,
	diminishing: value => Math.pow(value, 0.8)
}

const removeHandler = (object, key) => {
	if (key in object) {
		object[key]()
		delete object[key]
	}
}

const getMaxScrollTop = ctx => {
	const elem = ctx.elemRef.current
	return elem.scrollHeight - elem.height
}

const isScrollable = ctx => !ctx.props.isDisabled && getMaxScrollTop(ctx) > 0

const clampScrollTop = (ctx, scrollTop) =>
	Math.min(Math.max(scrollTop, 0), getMaxScrollTop(ctx))

const animateOverscroll = ctx => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	const toValue = clampScrollTop(ctx, scrollTop)
	if (toValue === scrollTop) return false
	Animated.spring(ctx.scrollTopValue, {
		toValue,
		bounciness: ctx.props.dragOverscrollBounciness,
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

const onChangeScrollSize = ctx => {
	const dragRecognizer = recognizer.recognizers.drag
	const isDisabled =
		cts.props.isDisabled || elemRef.scrollHeight <= elemRef.height
	isDisabled ? dragRecognizer.disable() : dragRecognizer.enable()
}

// Keyboard

// Which Scrollable currently handles key events
let currentKeyHandler

const makeCurrentKeyHandler = ctx => {
	if (currentKeyHandler) currentKeyHandler.removeKeyHandler()
	const document = ctx.elemRef.current.document
	ctx.removeKeyHandler = document.addKeyHandler(onKeyEvent.bind(null, ctx))
	currentKeyHandler = ctx
}

// When there are multiple nested scrollables, only top one should become active
let alreadySetKeyHandler = false

const onRawEvent = (ctx, event) => {
	if (!isScrollable(ctx)) return
	if (!alreadySetKeyHandler && isMouseButtonPress(event)) {
		makeCurrentKeyHandler(ctx)
		alreadySetKeyHandler = true
		setTimeout(() => {
			alreadySetKeyHandler = false
		}, 0)
	}
}

const onKeyEvent = (ctx, event) => {
	if (!isScrollable(ctx)) return
	if (event.action === KeyAction.PRESS || event.action === KeyAction.REPEAT) {
		if (event.key === KeyCode.UP || event.key == KeyCode.DOWN) {
			const scrollTop = ctx.scrollTopValue.__getValue()
			const delta =
				(event.key === KeyCode.UP ? -1 : 1) *
				ctx.props.keyboardScrollSpeed
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

// When there are multiple nested scrollables, only top one should be scrolled
const alreadyScrolled = false

const onScrollEvent = (ctx, event) => {
	if (!isScrollable(ctx) || alreadyScrolled) return
	const scrollTop = ctx.scrollTopValue.__getValue()
	const nextScrollTop = clampScrollTop(
		ctx,
		scrollTop + -1 * event.top * ctx.props.mousewheelScrollSpeed
	)
	animateScroll(ctx, nextScrollTop)
	alreadyScrolled = true
	setTimeout(() => {
		alreadyScrolled = false
	}, 0)
}

// Touch
const onDragStart = (ctx, event) => {
	if (!isScrollable(ctx)) return
	ctx.initialScrollTop = ctx.scrollTopValue.__getValue()
}

const onDragMove = (ctx, event) => {
	const maxScrollTop = getMaxScrollTop(ctx)
	let nextScrollTop = ctx.initialScrollTop - event.deltaTop
	const overscrollResistance = ctx.props.dragOverscrollResistance
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
		deceleration: ctx.props.dragDecayDeceleration
	}).start(({ finished }) => {
		ctx.scrollTopValue.removeListener(ctx.decayListener)
	})
}

const unmount = ctx => {
    ctx.scrollTopValue.stopAnimation()
	removeHandler(ctx, 'removeScrollHandler')
	removeHandler(ctx, 'removeKeyHandler')
	if (currentKeyHandler === ctx) {
		currentKeyHandler = undefined
		removeHandler(ctx, 'removeKeyHandler')
	}
}

const Scrollable = props => {
	const elemRef = useRef()
	const ctx = useContext({
		props,
		initialCtx: () => ({
			elemRef,
			initialScrollTop: 0,
			scrollTopValue: new Animated.Value(0),
			velocityTracker: new VelocityTracker()
		}),
		initialState: ctx => ({
			recognizer: new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: onHoverStart.bind(null, ctx),
					onHoverEnd: onHoverEnd.bind(null, ctx)
				}),
				drag: new DragRecognizer({
					document: () => ctx.elemRef.current.document,
					onDragStart: onDragStart.bind(null, ctx),
					onDragMove: onDragMove.bind(null, ctx),
					onDragEnd: onDragEnd.bind(null, ctx)
				}),
				raw: new RawRecognizer({
					handler: onRawEvent.bind(null, ctx)
				})
			})
		})
	})
	useEffect(() => {
		ctx.scrollTopValue.addListener(({ value }) => {
			elemRef.current.scrollTop = Math.round(value)
			ctx.velocityTracker.addPoint(Date.now(), value)
		})
		return unmount.bind(null, ctx)
	})
	const renderScroll = useCallback(() => (
		<Scroll
			ref={ctx.elemRef}
			onChangeScrollSize={onChangeScrollSize.bind(null, ctx)}
		>
			{ctx.props.children}
		</Scroll>
	))
	return (
		<Responder
			handler={useCallback((event, eventType) => {
				ctx.state.recognizer.handler(event, eventType)
			})}
		>
			{props.contentWrapper
				? React.createElement(props.contentWrapper, {
						children: renderScroll,
						scroll: ctx.elemRef.current
				  })
				: renderScroll()}
		</Responder>
	)
}

Scrollable.propTypes = {
	isDisabled: PropTypes.bool.isRequired,
	dragOverscrollBounciness: PropTypes.number.isRequired,
	dragOverscrollResistance: PropTypes.func.isRequired,
	dragDecayDeceleration: PropTypes.number.isRequired,
	keyboardScrollSpeed: PropTypes.number.isRequired,
	mousewheelScrollSpeed: PropTypes.number.isRequired,
	contentWrapper: PropTypes.func
}

Scrollable.defaultProps = {
	isDisabled: false,
	dragOverscrollBounciness: 6,
	dragOverscrollResistance: OverscrollResistance.diminishing,
	dragDecayDeceleration: 0.998,
	keyboardScrollSpeed: 50,
	mousewheelScrollSpeed: 100
}

export { OverscrollResistance }
export default Scrollable
