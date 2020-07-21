import {
    Alignment,
    BorderSide,
    BoxBorders,
    BoxRadiuses,
    Radius,
    Color,
    Value
} from '@advk/common'
import { Aligned, Background, Border, Sized, Stack } from '@advk/react-renderer'
import React, { useCallback, useMemo, useState } from 'react'

const ShadowBox = props => (
    <Sized
        sizeConstraints={{
            width: Value.abs(50),
            height: Value.abs(50)
        }}
    >
        <Border {...props}>
            <Background color={Color.lightGrey} />
        </Border>
    </Sized>
)

const ShadowExample = () => {
    return (
        <Stack>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(50))}
            >
                <ShadowBox shadows={[{ offset: { left: 10, top: 20 } }]} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(150))}
            >
                <ShadowBox shadows={[{ spread: 10 }]} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(250))}
            >
                <ShadowBox
                    radiuses={BoxRadiuses.all(Radius.circular(10))}
                    shadows={[{ offset: { left: 10, top: 10 } }]}
                />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(150), Value.abs(50))}
            >
                <ShadowBox shadows={[{ blur: 2 }]} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(150), Value.abs(150))}
            >
                <ShadowBox shadows={[{ blur: 5 }]} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(150), Value.abs(250))}
            >
                <ShadowBox shadows={[{ blur: 10 }]} />
            </Aligned>
        </Stack>
    )
}

export default ShadowExample
