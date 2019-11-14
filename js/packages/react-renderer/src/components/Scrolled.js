import React, {
    useRef,
    useEffect,
    useLayoutEffect,
    useImperativeHandle
} from 'react'
import { TransformMatrix } from '@advk/common'
import { Clip, Layer, CustomLayout } from '../nativeComponents'
import useContext from '../hooks/useContext.js'

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

const Scrolled = React.forwardRef((props, ref) => {
    const containerRef = useRef()
    const contentRef = useRef()
    const ctx = useContext({
        props,
        initialCtx: {
            containerRef,
            contentRef
        }
    })
    useEffect(() => {
        const container = containerRef.current
        const content = contentRef.current
        const document = container.document
        const unobserveContainer = document.observeElementSize(
            container,
            size => {
                if (ctx.height != size.height) {
                    ctx.height = size.height
                    ctx.props.onChangeHeight?.()
                }
            }
        )
        const unobserveContent = document.observeElementSize(content, size => {
            if (ctx.scrollHeight != size.height) {
                ctx.scrollHeight = size.height
                ctx.props.onChangeScrollHeight?.()
            }
        })
        return () => {
            unobserveContainer()
            unobserveContent()
        }
    }, [])
    useImperativeHandle(ref, () => ({
        get elem() {
            return ctx.containerRef.current
        },
        get document() {
            return this.elem.document
        },
        get scrollTop() {
            return ctx.scrollTop
        },
        set scrollTop(scrollTop) {
            if (scrollTop == ctx.scrollTop) return
            ctx.contentRef.current.transform = TransformMatrix.makeTranslate(
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
        <Clip ref={containerRef}>
            <Layer
                ref={contentRef}
                transform={TransformMatrix.makeTranslate(0, 0)}
            >
                <CustomLayout layout={layout}>{props.children}</CustomLayout>
            </Layer>
        </Clip>
    )
})

export default Scrolled
