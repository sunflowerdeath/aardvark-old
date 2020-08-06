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
import Input from './Input.js'

Promise.resolve().then(() => console.log('RESOLVE'))

const InputExample = () => {
    return (
        <Stack>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(50))}
            >
                <Input value='test'/>
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(150), Value.abs(50))}
            >
                <Input value='test'/>
            </Aligned>
        </Stack>
    )
}

export default InputExample
