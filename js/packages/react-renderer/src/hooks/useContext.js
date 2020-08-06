import { useEffect, useRef, useState } from 'react'

import useLastValue from './useLastValue.js'

const useFirstRender = () => {
    const isFirstRender = useRef(true)
    useEffect(() => {
        isFirstRender.current = false
    })
    return isFirstRender.current
}

// TODO better name
// Hook that creates object resembling `this` in old class components.
// It has properties `props`, `state` and `setState`, and can be used to store
// any properties.
const useContext = ({ initialCtx, props, initialState }) => {
    const isFirstRender = useFirstRender()
    const ref = useRef(
        typeof initialCtx === 'function' && isFirstRender
            ? initialCtx()
            : initialCtx
    )
    const ctx = ref.current
    const getProps = useLastValue(props)
    const [state, setState] = useState(
        typeof initialState === 'function' && isFirstRender
            ? initialState(ctx)
            : initialState
    )
    const getState = useLastValue(state)
    if (isFirstRender) {
        Object.defineProperty(ctx, 'props', { get: getProps })
        Object.defineProperty(ctx, 'state', { get: getState })
        Object.defineProperty(ctx, 'setState', {
            value: newState => {
                setState({ ...getState(), ...newState })
            }
        })
    }
    return ctx
}

export default useContext
