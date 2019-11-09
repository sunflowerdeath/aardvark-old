import React, { useState, useRef, useEffect } from 'react'
import { Value, Color, FlexDirection } from '@advk/common'
import ReactAardvark, {
    Align,
    Sized,
    Stack,
    Background,
    Flex,
    Text
} from '@advk/react-renderer'

import Button from './Button.js'

const ObserverExample = () => {
    const elemRef = useRef()
    const [width, setWidth] = useState(200)
    useEffect(() => {
        const elem = elemRef.current
        return elem.document.observeElementSize(elem, size =>
            log(JSON.stringify(size))
        )
    }, [])
    return (
        <Stack>
            <Align align={{ top: Value.abs(50), left: Value.abs(50) }}>
                <Sized
                    sizeConstraints={{
                        width: Value.abs(width),
                        height: Value.abs(50)
                    }}
                >
                    <Background color={Color.PURPLE} ref={elemRef} />
                </Sized>
            </Align>
            <Align align={{ top: Value.abs(20), right: Value.abs(20) }}>
                <Sized
                    sizeConstraints={{
                        width: Value.abs(200),
                        height: Value.abs(200)
                    }}
                >
                    <Stack>
                        <Background color={Color.LIGHTGREY} />
                        <Flex direction={FlexDirection.column}>
                            <Button onTap={() => setWidth(w => w + 10)}>
                                <Text text="+" />
                            </Button>
                            <Button onTap={() => setWidth(w => w - 10)}>
                                <Text text="-" />
                            </Button>
                        </Flex>
                    </Stack>
                </Sized>
            </Align>
        </Stack>
    )
}

export default ObserverExample
