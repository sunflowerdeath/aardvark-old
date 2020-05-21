import React from 'react'
import { Value, Padding, Radius, BoxRadiuses } from '@advk/common'
import {
    Stack,
    Translated,
    Sized,
    Border,
    Background
} from '../../nativeComponents.js'

const Ear = props => {
    const {
        left,
        top,
        width,
        height,
        side,
        barWidth,
        circleSize,
        color
    } = props

    const bar = (
        <Sized
            sizeConstraints={{ width: Value.abs(2), height: Value.abs(height) }}
        >
            <Background color={color} />
        </Sized>
    )

    const circle = (
        <Translated
            translation={{
                left: Value.abs((-circleSize + barWidth) / 2),
                top: Value.abs(side === 'end' ? height : -circleSize)
            }}
        >
            <Sized
                sizeConstraints={{
                    width: Value.abs(circleSize),
                    height: Value.abs(circleSize)
                }}
            >
                <Border
                    radiuses={BoxRadiuses.all(Radius.circular(circleSize / 2))}
                >
                    <Background color={color} />
                </Border>
            </Sized>
        </Translated>
    )

    const pos = {
        left:
            side === 'end'
                ? Value.abs(left + width - barWidth)
                : Value.abs(left),
        top: Value.abs(top)
    }

    return (
        <Translated translation={pos}>
            <Stack>
                {bar}
                {circle}
            </Stack>
        </Translated>
    )
}

Ear.defaultProps = {
    color: { red: 19, green: 111, blue: 225, alpha: 255 },
    circleSize: 10,
    barWidth: 2
}

export default Ear
