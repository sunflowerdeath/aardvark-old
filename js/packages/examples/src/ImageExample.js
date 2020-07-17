import React, { useState, useCallback, useMemo } from 'react'
import { Alignment, Value, Color, BoxBorders, BorderSide } from '@advk/common'
import { Aligned, Border, Sized, Stack, Image } from '@advk/react-renderer'

const ImageBox = props => (
    <Sized
        sizeConstraints={{
            width: Value.abs(100),
            height: Value.abs(100)
        }}
    >
        <Border borders={BoxBorders.all(BorderSide(1, Color.black))}>
            <Image src={FileDataSource('build/test2.png')} {...props} />
        </Border>
    </Sized>
)

const ImageExample = () => {
    return (
        <Stack>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(50))}
            >
                <ImageBox />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(200))}
            >
                <ImageBox fit={ImageFit.cover} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(350))}
            >
                <ImageBox fit={ImageFit.contain} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(50), Value.abs(500))}
            >
                <ImageBox fit={ImageFit.fill} />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(200), Value.abs(50))}
            >
                <ImageBox
                    src={FileDataSource('build/test3.png')}
                    fit={ImageFit.scaleDown}
                />
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(200), Value.abs(200))}
            >
                <ImageBox
                    src={FileDataSource('build/test4.png')}
                    fit={ImageFit.scaleDown}
                />
            </Aligned>
        </Stack>
    )
}

export default ImageExample
