import React, { useState, useRef, useEffect } from 'react'
import { Value, Color } from '@advk/common'
import ReactAardvark, {
    Align,
    Size,
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
        const conn = elem.document.observeElementSize(elem, size =>
            log(JSON.stringify(size))
        )
        return () => conn.disconnect()
    }, [])
    return (
        <Stack>
            <Align insets={{ top: Value.abs(50), left: Value.abs(50) }}>
                <Size
                    sizeConstraints={{
                        width: Value.abs(width),
                        height: Value.abs(50)
                    }}
                >
                    <Background color={Color.PURPLE} ref={elemRef} />
                </Size>
            </Align>
            <Align insets={{ top: Value.abs(20), right: Value.abs(20) }}>
                <Size
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
                </Size>
            </Align>
        </Stack>
    )
}

export default ObserverExample
