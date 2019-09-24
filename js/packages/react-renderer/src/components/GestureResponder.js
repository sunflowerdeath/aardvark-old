import React, { useState, useEffect, useRef, useCallback } from 'react'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import TapRecognizer from '@advk/common/src/gestures/TapRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import { Responder } from '../nativeComponents.js'

const useLastProps = props => {
	const ref = useRef({})
	useEffect(() => {
		ref.current = props
	}, [props])
	return ref
}

const GestureResponder = props => {
	const { children } = props

	const lastProps = useLastProps(props)
	const ref = useRef()
	const [recognizer] = useState(() => {
		const callbackCall = name => event => {
			if (typeof lastProps.current[name] === 'function') {
                lastProps.current[name](event)
            }
		}
		return new MultiRecognizer({
			hover: new HoverRecognizer({
				onHoverStart: callbackCall('onHoverStart'),
				onHoverEnd: callbackCall('onHoverEnd')
			}),
			tap: new TapRecognizer({
				document: () => ref.current.document,
				onPressStart: callbackCall('onPressStart'),
				onPressEnd: callbackCall('onPressEnd'),
				onTap: callbackCall('onTap')
			})
		})
	})

	const handler = useCallback(recognizer.handler.bind(recognizer), [
		recognizer
	])

	return (
		<Responder
			handler={handler}
			ref={ref}
		>
			{children}
		</Responder>
	)
}

export default GestureResponder
