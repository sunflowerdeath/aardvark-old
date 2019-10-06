import React, { useState, useRef, useCallback, useEffect } from 'react'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import { Responder, Scroll } from '../nativeComponents'
import KeyCode from './KeyCode.js'

const KeyAction = {
	PRESS: 0,
	RELEASE: 1,
	REPEAT: 2
}

const useStateWithGetter = initialState => {
	const [state, setState] = useState(initialState)
	const ref = useRef(initialState)
	useEffect(() => {
		ref.current = state
	})
	return [state, setState, () => ref.current]
}

const springToEdge = ctx => {
	Animated.spring(ctx.scrollTopValue, { toValue: 0, damping: 10 }).start()
}

const onDragEnd = (ctx, event) => {
	if (ctx.scrollTopValue.__getValue() < 0) {
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
	// while decaying - update the velocity
	// when decay becomes < 0 - stop animation and run spring with current
	// velocity
	log('decay start')
}

const KEYBOARD_SCROLL_DISTANCE = 10

const onKeyEvent = (ctx, event) => {
	if (event.action === KeyAction.PRESS || event.action === KeyAction.REPEAT) {
		if (event.key === KeyCode.UP || event.key == KeyCode.DOWN) {
			const nextScrollTop =
				ctx.scrollTopValue.__getValue() +
				(event.key === KeyCode.UP ? 1 : -1) * KEYBOARD_SCROLL_DISTANCE
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
		scrollTopValue: new Animated.Value(0)
	})
	useEffect(() =>
		ctx.current.scrollTopValue.addListener(({ value }) => {
			ref.current.scrollTop = Math.round(value)
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
