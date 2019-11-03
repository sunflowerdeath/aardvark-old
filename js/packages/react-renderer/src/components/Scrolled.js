import React, { useMemo } from 'react'
import { TransformMatrix } from '@advk/common'
import { Clip, Layer, CustomLayout } from '../nativeComponents'

const layout = (elem, constraints) => {
    let scrollHeight = 0
    for (const child of elem.children) {
        const childConstraints = {
            minWidth: 0,
            maxWidth: constraints.maxWidth,
            minHeight: 0,
            maxHeight: Infinity
        }
        const childSize = elem.document.layoutElement(child, childConstraints)
        child.setLayoutProps({ left: 0, top: scrollHeight }, childSize)
        scrollHeight += childSize.height
    }
    return { width: constraints.maxWidth, height: scrollHeight }
}

const Scrolled = props => {
    const { children, scrollTop } = props
    const transform = useMemo(
        () => TransformMatrix.makeTranslate(0, -scrollTop),
        [scrollTop]
    )
    return (
        <Clip>
            <Layer transform={transform}>
                <CustomLayout layout={layout}>{children}</CustomLayout>
            </Layer>
        </Clip>
    )
}

Scrolled.defaultProps = {
    scrollTop: 0
}

export default Scrolled
