const elements = {
	align: Align,
	background: Background,
	center: Center,
	intrinsicHeight: IntrinsicHeight,
	intrinsicWidth: IntrinsicWidth,
	flex: Flex,
	flexChild: FlexChild,
	padding: Padding,
	responder: Responder,
	scroll: Scroll,
	sized: Sized,
	stack: Stack,
	text: Text,
	translate: Translate
}

const createElement = (type, props) => {
    if (!(type in elements)) {
        return new Error(`Invalid element type \`${type}\`.`)
    }
	const elem = new elements[type]()
	for (const key in props) {
		if (key === 'children') continue
		elem[key] = props[key]
	}
    return elem
}

const updateElement = (elem, type, oldProps, newProps) => {
    for (const key in oldProps) {
        if (key === 'children') continue
        if (!(key in newProps)) delete elem[key] // TODO or set to undefined
    }
    for (const key in newProps) {
        if (key === 'children') continue
        if (newProps[key] !== oldProps[key]) {
            log('UPDATE ' + type + ': ' + key)
            elem[key] = newProps[key]
        }
    }
}

export { createElement, updateElement }
