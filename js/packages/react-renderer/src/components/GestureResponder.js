import React, { useRef } from 'react'
import useMultiRecognizer from '../hooks/useMultiRecognizer'
import { Responder } from '../nativeComponents'

const GestureResponder = props => {
    const { children, ...recognizerProps } = props
    const ref = useRef()
    const handler = useMultiRecognizer(ref, recognizerProps)
    return <Responder handler={handler} ref={ref}>{children}</Responder>
}

export default GestureResponder
