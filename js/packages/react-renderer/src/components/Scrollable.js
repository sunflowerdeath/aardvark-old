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

const springToEdge = ctx => {
	Animated.spring(ctx.scrollTopValue, {
		toValue: 0,
		bounciness: 6,
        // velocity is based on seconds instead of milliseconds
		velocity: ctx.velocityTracker.getVelocity() * 1000
	}).start()
}

const onDragEnd = (ctx, event) => {
	const scrollTop = ctx.scrollTopValue.__getValue()
	if (scrollTop < 0) {
		springToEdge(ctx)
		return
	}

	ctx.decayListener = ctx.scrollTopValue.addListener(({ value }) => {
		if (value < 0) springToEdge(ctx)
	})
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
					onDragMove: event => {
						ctx.current.scrollTopValue.setValue(
							ctx.current.initialScrollTop - event.deltaTop
						)
					},
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
