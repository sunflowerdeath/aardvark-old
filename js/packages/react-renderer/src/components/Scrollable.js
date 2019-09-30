import React, { useState, useRef, useCallback, useEffect } from 'react'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import { Responder, Scroll } from '../nativeComponents'

import Animated from 'animated'

const useStateWithGetter = initialState => {
	const [state, setState] = useState(initialState)
	const ref = useRef(initialState)
	useEffect(() => {
		ref.current = state
	})
	return [state, setState, () => ref.current]
}

const onDragEnd = (ctx, event) => {
	try {
		log('velocity', event.velocity)

		if (ctx.scrollTopValue.__getValue() < 0) {
			Animated.spring(ctx.scrollTopValue, {
				toValue: 0
			}).start()
			return
		}

		Animated.decay(ctx.scrollTopValue, { velocity: -event.velocity }).start(
			({ finished }) => {
				log('decay end', finished, ctx.scrollTopValue.__getValue())
				if (!finished) return
				if (ctx.scrollTopValue.__getValue() < 0) {
					Animated.spring(ctx.scrollTopValue, {
						toValue: 0
					}).start()
					log('spring')
				}
			}
		)
	} catch (e) {
		log(e)
	}
	log('decay start')
}

const Scrollable = ({ children }) => {
	const [scrollTop, setScrollTop, getScrollTop] = useStateWithGetter(0)
	const ref = useRef()
	const ctx = useRef({
		initialScrollTop: 0,
		scrollTopValue: new Animated.Value(0)
	})
	useEffect(() =>
		ctx.current.scrollTopValue.addListener(({ value }) => {
			// log('update', Math.round(value))
			ref.current.scrollTop = Math.round(value)
		})
	)
	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: () => log('hover'),
					onHoverEnd: () => log('hover end')
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
