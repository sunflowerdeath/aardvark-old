import React, { useState, useEffect, useRef, useCallback } from 'react'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import TapRecognizer from '@advk/common/src/gestures/TapRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import useLastValue from '../hooks/useLastValue.js'

// Hook that composes different handler functions from props into single handler
// for the Responder element
const useMultiRecognizer = (ref, recognizerProps) => {
    const getRecognizerProps = useLastValue(recognizerProps)
    const [recognizer] = useState(() => {
        const makeCallback = name => event => {
            if (typeof getRecognizerProps()[name] === 'function') {
                getRecognizerProps()[name](event)
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

    const didUnmountRef = useRef(false)
    useEffect(() => {
        return () => {
            recognizer.destroy()
            didUnmountRef.current = true
        }
    }, [])

    const handler = useCallback((...args) => {
        if (!didUnmountRef.current) recognizer.handler(...args)
    }, [])
    return handler
}

export default useMultiRecognizer
