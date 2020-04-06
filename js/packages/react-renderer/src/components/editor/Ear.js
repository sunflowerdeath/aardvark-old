import React from 'react'
import { Value, Padding, Radius, BoxRadiuses } from '@advk/common'
import {
    Stack,
    Translate,
    Size,
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
        <Size
            sizeConstraints={{ width: Value.abs(2), height: Value.abs(height) }}
        >
            <Background color={color} />
        </Size>
    )

    const circle = (
        <Translate
            translation={{
                left: Value.abs((-circleSize + barWidth) / 2),
                top: Value.abs(side === 'end' ? height : -circleSize)
            }}
        >
            <Size
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
            </Size>
        </Translate>
    )

    const pos = {
        left:
            side === 'end'
                ? Value.abs(left + width - barWidth)
                : Value.abs(left),
        top: Value.abs(top)
    }

    return (
        <Translate translation={pos}>
            <Stack>
                {bar}
                {circle}
            </Stack>
        </Translate>
    )
}

Ear.defaultProps = {
    color: { red: 19, green: 111, blue: 225, alpha: 255 },
    circleSize: 10,
    barWidth: 2
}

export default Ear
