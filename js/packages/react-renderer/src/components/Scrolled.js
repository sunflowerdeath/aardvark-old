import React, { useMemo, useCallback, useRef, useImperativeHandle } from 'react'
import { TransformMatrix } from '@advk/common'
import { Clip, Layer, CustomLayout } from '../nativeComponents'
import useContext from '../hooks/useContext.js'

const containerLayout = (ctx, elem, constraints) => {
    const child = elem.children[0]
    const size = elem.document.layoutElement(child, constraints)
    child.setLayoutProps({ left: 0, top: 0 }, size)
    if (ctx.heigth != size.height) {
        ctx.height = size.height
        ctx.props.onChangeSize()
    }
    return size
}

const contentLayout = (ctx, elem, constraints) => {
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
    if (ctx.scrollHeight != scrollHeight) {
        ctx.scrollHeight = scrollHeight
        ctx.props.onChangeSize()
    }
    return { width: constraints.maxWidth, height: scrollHeight }
}

const Scrolled = React.forwardRef((props, ref) => {
    const layerRef = useRef()
    const ctx = useContext({
        props,
        initialCtx: {
            layerRef
        }
    })
    const contentLayoutCb = useCallback((elem, constraints) =>
        contentLayout(ctx, elem, constraints)
    )
    const containerLayoutCb = useCallback((elem, constraints) =>
        containerLayout(ctx, elem, constraints)
    )
    useImperativeHandle(ref, () => ({
        get elem() {
            return ctx.layerRef.current
        },
        get document() {
            return this.elem.document
        },
        get scrollTop() {
            return ctx.scrollTop
        },
        set scrollTop(scrollTop) {
            if (scrollTop == ctx.scrollTop) return
            ctx.layerRef.current.transform = TransformMatrix.makeTranslate(
                0,
                -scrollTop
            )
            ctx.scrollTop = scrollTop
        },
        get scrollHeight() {
            return ctx.scrollHeight
        },
        get height() {
            return ctx.height
        }
    }))
    return (
        <CustomLayout layout={containerLayoutCb}>
            <Clip>
                <Layer
                    ref={layerRef}
                    transform={TransformMatrix.makeTranslate(0, 0)}
                >
                    <CustomLayout layout={contentLayoutCb}>
                        {props.children}
                    </CustomLayout>
                </Layer>
            </Clip>
        </CustomLayout>
    )
})

export default Scrolled
