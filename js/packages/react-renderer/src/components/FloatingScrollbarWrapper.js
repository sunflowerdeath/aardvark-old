import React, { useRef, useEffect } from 'react'
import PropTypes from 'prop-types'
import Animated from 'animated'
import { Value, TransformMatrix } from '@advk/common'
import {
    Align,
    Sized,
    Stack,
    Background,
    Clip,
    Layer
} from '../nativeComponents'
import useContext from '../hooks/useContext.js'

const DefaultFloatingScrollbar = () => (
    <Background color={{ red: 0, blue: 0, green: 0, alpha: 128 }} />
)

const animateOpacity = (ctx, opacity) =>
    Animated.spring(ctx.opacityValue, {
        toValue: opacity,
        bounciness: 0
    }).start()

const FloatingScrollbarWrapper = props => {
    const {
        children,
        height,
        scrollHeight,
        scrollTopValue,
        scrollbarComponent,
        autoHide
    } = props
    const layerRef = useRef()
    const ctx = useContext({
        props,
        initialCtx: {
            layerRef,
            opacityValue: new Animated.Value(0)
        }
    })
    useEffect(() => {
        scrollTopValue.addListener(({ value }) => {
            const top = (height * value) / scrollHeight
            layerRef.current.transform = TransformMatrix.makeTranslate(0, top)
            if (ctx.props.autoHide) {
                clearTimeout(ctx.hideTimer)
                animateOpacity(ctx, 1)
                ctx.hideTimer = setTimeout(
                    () => animateOpacity(ctx, 0),
                    ctx.props.hideTimeout
                )
            }
        })
    }, [height, scrollHeight])
    useEffect(() => {
        if (ctx.props.autoHide) ctx.layerRef.current.opacity = 0
        ctx.opacityValue.addListener(({ value }) => {
            ctx.layerRef.current.opacity = value
        })
        return () => ctx.opacityValue.stopAnimation()
    }, [])
    const scrollbarHeight = height * (height / scrollHeight)
    return (
        <Stack>
            {children}
            <Align alignment={{ top: Value.abs(0), right: Value.abs(0) }}>
                <Sized
                    sizeConstraints={{
                        height: Value.rel(1),
                        width: Value.abs(10)
                    }}
                >
                    <Clip>
                        <Layer ref={layerRef}>
                            <Sized
                                sizeConstraints={{
                                    height: Value.abs(scrollbarHeight),
                                    width: Value.rel(1)
                                }}
                            >
                                {React.createElement(scrollbarComponent)}
                            </Sized>
                        </Layer>
                    </Clip>
                </Sized>
            </Align>
        </Stack>
    )
}

FloatingScrollbarWrapper.propTypes = {
    scrollbarWidth: PropTypes.number.isReqiured,
    autoHide: PropTypes.bool.isRequired,
    hideTimeout: PropTypes.number.isRequired,
    scrollbarComponent: PropTypes.elementType.isRequired
}

FloatingScrollbarWrapper.defaultProps = {
    scrollbarWidth: 8,
    autoHide: true,
    hideTimeout: 1000,
    scrollbarComponent: DefaultFloatingScrollbar
}

export default FloatingScrollbarWrapper
