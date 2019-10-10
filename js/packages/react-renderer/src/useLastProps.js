import { useEffect, useRef } from 'react'

const useLastProps = props => {
	const ref = useRef({})
	useEffect(() => {
		ref.current = props
	}, [props])
	return () => ref.current
}

export default useLastProps
