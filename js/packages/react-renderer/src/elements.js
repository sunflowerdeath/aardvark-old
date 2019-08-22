import check, { CheckError } from '@advk/check-types'

const componentType = 1 // TODO

const childrenTypes = {
    none: check.enum([undefined]),
    single: componentType,
    multiple: check.arrayOf(componentType),
    text: check.string
}

const valueType = check.shape({
	value: check.number,
	type: check.enum(['abs', 'rel'])
})

const sizeConstraintsType = check.shape({
	minWidth: valueType,
	minHeight: valueType,
	maxWidth: valueType,
	maxHeight: valueType
    /*
	width: valueType,
	height: valueType,
	minWidth: check.optional(valueType),
	minHeight: check.optional(valueType),
	maxWidth: check.optional(valueType),
	maxHeight: check.optional(valueType)
    */
})

const alignmentType = check.shape({
	left: valueType,
	top: valueType
})

const colorValueType = (value, name, location) => {
	const isValid = typeof name === 'number' || (value >= 0 && value <= 255)
	if (!isValid) {
		return new CheckError(
			`Invalid property ${name} of value ${value} supplied to ` +
				`${location}. Required number between 0 to 255.`
		)
	}
}

const colorType = check.shape({
	alpha: colorValueType,
	red: colorValueType,
	green: colorValueType,
	blue: colorValueType
})

const elements = {
	align: {
		element: Align,
		props: {
			align: alignmentType
		},
		children: childrenTypes.single
	},
	background: {
		element: Background,
		props: {
			color: colorType
		},
		children: childrenTypes.none
	},
	center: {
		element: Center,
		props: {},
		children: childrenTypes.single
	},
	responder: {
		element: Responder,
		props: {
			handler: check.func
		},
		children: childrenTypes.single
	},
	sized: {
		element: Sized,
		props: {
			sizeConstraints: sizeConstraintsType
		},
		children: childrenTypes.single
	},
	stack: {
		element: Stack,
		props: {},
		children: childrenTypes.multiple
	},
	text: {
		element: Text,
		props: {
			text: check.string
		},
		children: childrenTypes.text
	}
}

const createElement = (type, props) => {
    if (!(type in elements)) {
        return new Error(`Invalid element type \`${type}\`.`)
    }
	const config = elements[type]
    // TODO better children
	const error = check.shape({ ...config.props, children: () => {} })(props)
	if (error) throw error
	const elem = new config.element()
	for (let key in props) {
		if (key === 'children') continue
		elem[key] = props[key]
	}
    return elem
    // TODO validate child
}

const updateElement = (elem, type, oldProps, newProps) => {
	const config = elements[type]
    // const error = check.shape(config.props)(newProps)
    // if (error) throw error
    for (let key in oldProps) {
        if (key === 'children') continue
        if (!(key in newProps)) delete elem[key] // or set to undefined ??
    }
    for (let key in newProps) {
        if (key === 'children') continue
        if (newProps[key] !== oldProps[key]) elem[key] = newProps[key]
    }
}

export { createElement, updateElement }
