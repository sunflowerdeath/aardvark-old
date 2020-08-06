const useCombinedRefs = (...refs) => value => {
    refs.forEach(ref => {
        if (!ref) return
        if (typeof ref === 'function') {
            ref(value)
        } else {
            ref.current = value
        }
    })
}

export default useCombinedRefs
