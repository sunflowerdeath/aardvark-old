import React, { useState, useEffect, useRef, useCallback } from 'react'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import TapRecognizer from '@advk/common/src/gestures/TapRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import { Responder } from '../nativeComponents.js'
import useLastValue from '../hooks/useLastValue.js'

const GestureResponder = props => {
	const { children } = props

	const getProps = useLastValue(props)
	const ref = useRef()
	const [recognizer] = useState(() => {
		const makeCallback = name => event => {
			if (typeof getProps()[name] === 'function') {
                getProps()[name](event)
            }
		}
		return new MultiRecognizer({
			hover: new HoverRecognizer({
				onHoverStart: makeCallback('onHoverStart'),
				onHoverEnd: makeCallback('onHoverEnd')
			}),
			tap: new TapRecognizer({
				document: () => ref.current.document,
				onPressStart: makeCallback('onPressStart'),
				onPressEnd: makeCallback('onPressEnd'),
				onTap: makeCallback('onTap')
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
