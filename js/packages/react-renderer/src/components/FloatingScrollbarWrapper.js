import React, { useRef, useEffect } from 'react'
import { Value, TransformMatrix } from '@advk/common'
import {
    Align,
    Sized,
    Stack,
    Background,
    Clip,
    Layer
} from '../nativeComponents'

const DefaultFloatingScrollbar = () => (
    <Background color={{ red: 0, blue: 0, green: 0, alpha: 128 }} />
)

const FloatingScrollbarWrapper = props => {
    const {
        children,
        height,
        scrollHeight,
        scrollTopValue,
        scrollbarComponent
    } = props
    const scrollbarHeight = height * (height / scrollHeight)
    const layerRef = useRef()
    useEffect(() => {
        scrollTopValue.addListener(({ value }) => {
            const top = (height * value) / scrollHeight
            layerRef.current.transform = TransformMatrix.makeTranslate(0, top)
        })
    }, [height, scrollHeight])
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
                        <Layer
                            ref={layerRef}
                            transform={TransformMatrix.makeTranslate(0, 0)}
                        >
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

FloatingScrollbarWrapper.defaultProps = {
    scrollbarComponent: DefaultFloatingScrollbar
}

export default FloatingScrollbarWrapper
