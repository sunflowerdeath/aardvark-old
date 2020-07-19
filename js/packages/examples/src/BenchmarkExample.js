import React, { useState, useCallback, useMemo } from 'react'
import { Alignment, Value, Color } from '@advk/common'
import { Aligned, Sized, Stack, Background } from '@advk/react-renderer'

const randInt = limit => Math.round(Math.random() * limit)

const colors = []
for (let i = 0; i < 400; i++)
    colors.push(Color.rgb(randInt(255), randInt(255), randInt(255)))

const BenchmarkExample = () => {
    let elems = []
    for (let row = 0; row < 20; row++) {
        for (let col = 0; col < 20; col++) {
            elems.push(
                <Aligned
                    alignment={Alignment.topLeft(
                        Value.abs(row * 20),
                        Value.abs(col * 25)
                    )}
                >
                    <Sized
                        sizeConstraints={{
                            width: Value.abs(20),
                            height: Value.abs(15)
                        }}
                    >
                        <Background color={colors[row * 20 + col]} />
                    </Sized>
                </Aligned>
            )
        }
    }
    return <Stack>{elems}</Stack>
}

export default BenchmarkExample
