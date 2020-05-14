import React, { useRef } from 'react'
import useMultiRecognizer from '../hooks/useMultiRecognizer'
import { ResponderSpanC } from '../nativeComponents.js'

const GestureResponderSpan = props => {
    const { children, ...recognizerProps } = props
    const ref = useRef()
    const handler = useMultiRecognizer(ref, recognizerProps)
    return (
        <ResponderSpanC handler={handler} ref={ref}>
            {children}
        </ResponderSpanC>
    )
}

export default GestureResponderSpan
