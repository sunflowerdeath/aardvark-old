import React, { useState, useCallback, useMemo } from 'react'
import { Alignment, Value, Color, TransformMatrix } from '@advk/common'
import {
    Align as Aligned,
    Size,
    Stack,
    Background,
    Layer,
    Flex,
    Text,
    GestureResponder
} from '@advk/react-renderer'

import Button from './Button.js'

const toRad = deg => (deg * Math.PI) / 180

const INITIAL_COLOR = { red: 179, green: 229, blue: 252, alpha: 255 }
const HOVERED_COLOR = { red: 79, green: 195, blue: 247, alpha: 255 }

const LayerExample = () => {
    const [isHovered, setIsHovered] = useState(false)
    const [translate, setTranslate] = useState(0)
    const [rotate, setRotate] = useState(45)
    const matrix = useMemo(
        () =>
            TransformMatrix.append(
                TransformMatrix.makeTranslate(0, translate),
                TransformMatrix.makeRotate(toRad(rotate), 50, 50)
            ),
        [translate, rotate]
    )
    return (
        <Stack>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(50))}
            >
                <Layer transform={matrix}>
                    <GestureResponder
                        onHoverStart={useCallback(() => setIsHovered(true))}
                        onHoverEnd={useCallback(() => setIsHovered(false))}
                    >
                        <Size
                            sizeConstraints={{
                                width: Value.abs(100),
                                height: Value.abs(100)
                            }}
                        >
                            <Background
                                color={
                                    isHovered ? HOVERED_COLOR : INITIAL_COLOR
                                }
                            />
                        </Size>
                    </GestureResponder>
                </Layer>
            </Aligned>
            <Aligned
                alignment={Alignment.topRight(Value.abs(20), Value.abs(20))}
            >
                <Size
                    sizeConstraints={{
                        width: Value.abs(200),
                        height: Value.abs(200)
                    }}
                >
                    <Stack>
                        <Background color={Color.LIGHTGREY} />
                        <Flex direction={FlexDirection.column}>
                            <Button onTap={() => setTranslate(t => t - 5)}>
                                <Text text="/\" />
                            </Button>
                            <Button onTap={() => setTranslate(t => t + 5)}>
                                <Text text="\/" />
                            </Button>
                            <Button onTap={() => setRotate(r => r - 5)}>
                                <Text text="<-" />
                            </Button>
                            <Button onTap={() => setRotate(r => r + 5)}>
                                <Text text="->" />
                            </Button>
                        </Flex>
                    </Stack>
                </Size>
            </Aligned>
        </Stack>
    )
}

export default LayerExample
