import React from 'react'

const PREFIX = 'adv_'

const nativeComponents = {}

const registerNativeComponent = (name, elementClass) => {
    const id = PREFIX + name
    nativeComponents[id] = elementClass
    const NativeComponent = React.forwardRef((props, ref) =>
        React.createElement(id, { ...props, ref })
    )
    return NativeComponent
}

const createElement = (type, props) => {
    if (!(type in nativeComponents)) {
        return new Error(`Invalid element type \`${type}\`.`)
    }
    const elem = new nativeComponents[type]()
    for (const key in props) {
        if (key === 'children') continue
        if (key === 'ref') {
            props[key](elem)
            continue
        }
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
        if (key === 'ref') {
            props[key](elem)
            continue
        }
        if (newProps[key] !== oldProps[key]) {
            elem[key] = newProps[key]
        }
    }
}

export { registerNativeComponent, createElement, updateElement }
