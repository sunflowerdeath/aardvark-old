import React, { useState, useRef, useCallback, useEffect } from 'react'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import { Responder, Scroll } from '../nativeComponents'

const useStateWithGetter = initialState => {
	const [state, setState] = useState(initialState)
	const ref = useRef(initialState)
	useEffect(() => {
		ref.current = state
	})
	return [state, setState, () => ref.current]
}

const Scrollable = ({ children }) => {
	const [scrollTop, setScrollTop, getScrollTop] = useStateWithGetter(0)
	const initialScrollTop = useRef(0)
	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: () => log('hover'),
					onHoverEnd: () => log('hover end')
				}),
				drag: new DragRecognizer({
					onDragStart: event => {
						log('drag start')
						initialScrollTop.current = getScrollTop()
					},
					onDragMove: event => {
						log('drag move')
						setScrollTop(initialScrollTop.current + event.deltaTop)
					}
				})
			})
	)
    log(scrollTop)
	return (
		<Responder handler={useCallback(recognizer.handler.bind(recognizer))}>
			<Scroll scrollTop={scrollTop}>{children}</Scroll>
		</Responder>
	)
}

export default Scrollable
