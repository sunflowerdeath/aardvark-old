import React, { useState, useCallback } from 'react'
import { Color, Value, Padding as Padding1 } from '@advk/common'
import ReactAardvark, {
    GestureResponder,
    Sized,
    Stack,
    Background,
    Center,
    Padding,
    IntrinsicWidth
} from '@advk/react-renderer'

const BTN_INITIAL_COLOR = { red: 238, green: 238, blue: 238, alpha: 255 }
const BTN_HOVERED_COLOR = { red: 165, green: 214, blue: 167, alpha: 255 }
const BTN_PRESSED_COLOR = { red: 255, green: 171, blue: 145, alpha: 255 }

const Button = ({ children, onTap }) => {
    const [isPressed, setIsPressed] = useState(false)
    const [isHovered, setIsHovered] = useState(false)
    return (
        <GestureResponder
            onTap={onTap}
            onPressStart={useCallback(() => setIsPressed(true))}
            onPressEnd={useCallback(() => setIsPressed(false))}
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
        >
            <Sized sizeConstraints={{ height: Value.abs(40) }}>
                <Stack>
                    <Background
                        color={
                            isPressed
                                ? BTN_PRESSED_COLOR
                                : isHovered
                                ? BTN_HOVERED_COLOR
                                : BTN_INITIAL_COLOR
                        }
                    />
                    <Center>
                        <Padding padding={Padding1.horiz(16)}>
                            <IntrinsicWidth>{children}</IntrinsicWidth>
                        </Padding>
                    </Center>
                </Stack>
            </Sized>
        </GestureResponder>
    )
}

export default Button
