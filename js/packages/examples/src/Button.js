import React, { useState, useCallback } from 'react'
import { Color, Value, Insets } from '@advk/common'
import ReactAardvark, {
    GestureResponder,
    Sized,
    Stack,
    Background,
    Center,
    Padded,
    IntrinsicWidth
} from '@advk/react-renderer'

const INITIAL_COLOR = { red: 238, green: 238, blue: 238, alpha: 255 }
const HOVERED_COLOR = { red: 165, green: 214, blue: 167, alpha: 255 }
const PRESSED_COLOR = { red: 255, green: 171, blue: 145, alpha: 255 }

const Button = ({ children, onTap }) => {
    const [isPressed, setIsPressed] = useState(false)
    const [isHovered, setIsHovered] = useState(false)
    const color = isPressed
        ? PRESSED_COLOR
        : isHovered
        ? HOVERED_COLOR
        : INITIAL_COLOR
    return (
        <GestureResponder
            onTap={onTap}
            onPressStart={useCallback(() => setIsPressed(true))}
            onPressEnd={useCallback(() => setIsPressed(false))}
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
        >
            <Sized sizeConstraints={{ height: Value.abs(40) }}>
                <IntrinsicWidth>
                    <Stack>
                        <Background color={color} />
                        <Center>
                            <Padded padding={Insets.horiz(16)}>
                                {children}
                            </Padded>
                        </Center>
                    </Stack>
                </IntrinsicWidth>
            </Sized>
        </GestureResponder>
    )
}

export default Button
