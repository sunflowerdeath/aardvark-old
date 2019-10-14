import { useEffect, useRef, useState } from 'react'

import useLastValue from './useLastValue.js'

// Hook that creates object resembling `this` in old class components.
// It has properties `props`, `state` and `setState`, and can be used to store 
// any properties.
const useContext = ({ initialCtx, props, initialState }) => {
	const ref = useRef(
		typeof initialCtx === 'function' ? initialCtx() : initialCtx
	)
	const ctx = ref.current
	const getProps = useLastValue(props)
	const [state, setState] = useState(
		typeof initialState === 'function' ? initialState(ctx) : initialState
	)
	const getState = useLastValue(state)
	useEffect(() => {
        Object.defineProperty(ctx, 'props', { get: getProps })
        Object.defineProperty(ctx, 'state', { get: getState })
        Object.defineProperty(ctx, 'setState', { value: setState })
	})
	return ctx
}

export default useContext
