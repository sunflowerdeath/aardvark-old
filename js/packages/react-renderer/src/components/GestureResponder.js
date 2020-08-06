import React, { forwardRef, useRef } from 'react'
import useMultiRecognizer from '../hooks/useMultiRecognizer'
import useCombinedRefs from '../hooks/useCombinedRefs'
import { Responder } from '../nativeComponents'

const GestureResponder = React.forwardRef((props, ref) => {
    const { children, ...recognizerProps } = props
    const innerRef = useRef()
    const handler = useMultiRecognizer(innerRef, recognizerProps)
    return <Responder handler={handler} ref={useCombinedRefs(ref, innerRef)}>{children}</Responder>
})

export default GestureResponder
