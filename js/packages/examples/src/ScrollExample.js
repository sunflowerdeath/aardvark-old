import React, { useState, useCallback } from 'react'
import {
    Color,
    Value,
    Padding as Padding1,
    FlexDirection,
    PointerEventType
} from '@advk/common'
import ReactAardvark, {
    Align,
    GestureResponder,
    Sized,
    Stack,
    Background,
    Center,
    Padding,
    Responder,
    IntrinsicWidth,
    IntrinsicHeight,
    Flex,
    Text
} from '@advk/react-renderer'
import Scrollable from '@advk/react-renderer/src/components/Scrollable'
import Scrolled from '@advk/react-renderer/src/components/Scrolled'
import Button from './Button.js'

const INITIAL_COLOR = { red: 179, green: 229, blue: 252, alpha: 255 }
const HOVERED_COLOR = { red: 79, green: 195, blue: 247, alpha: 255 }

const Panel = ({ children }) => {
    const [isHovered, setIsHovered] = useState(false)
    return (
        <GestureResponder
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
        >
            <IntrinsicHeight>
                <Sized sizeConstraints={{ width: Value.abs(200) }}>
                    <Stack>
                        <Background
                            color={isHovered ? HOVERED_COLOR : INITIAL_COLOR}
                        />
                        <Padding padding={Padding1.all(16)}>
                            <Flex direction={FlexDirection.column}>
                                {children}
                            </Flex>
                        </Padding>
                    </Stack>
                </Sized>
            </IntrinsicHeight>
        </GestureResponder>
    )
}

const range = (from, to) => {
    const res = []
    for (let i = from; i <= to; i++) res.push(i)
    return res
}

const ListItem = ({ children }) => {
    const [isHovered, setIsHovered] = useState(false)

    const handler = (event, eventType) => {
        if (eventType === PointerEventType.ENTER) setIsHovered(true)
        else if (eventType === PointerEventType.LEAVE) setIsHovered(false)
    }
    return (
        <Responder handler={handler}>
            <IntrinsicHeight>
                <Stack>
                    <Background
                        color={isHovered ? Color.GREEN : Color.TRANSPARENT}
                    />
                    <Padding padding={Padding1.all(16)}>
                        <Text text={children.join('')} />
                    </Padding>
                </Stack>
            </IntrinsicHeight>
        </Responder>
    )
}

const ContentWrapper = ({ children, elem }) => {
    return (
        <Stack>
            {children()}
            <Align alignment={{ top: Value.abs(0), right: Value.abs(0) }}>
                <Sized
                    sizeConstraints={{
                        height: Value.rel(1),
                        width: Value.abs(10)
                    }}
                >
                    <Background
                        color={{ red: 255, blue: 0, green: 0, alpha: 128 }}
                    />
                </Sized>
            </Align>
        </Stack>
    )
}

const ScrollExample = () => {
    const [scrollTop, setScrollTop] = useState(0)
    return (
        <Padding padding={Padding1.symmetrical(20, 40)}>
            <Flex>
                <Flex direction={FlexDirection.column}>
                    <Padding padding={Padding1.only('bottom', 16)}>
                        <Panel>
                            <Padding padding={Padding1.only('bottom', 16)}>
                                <Button onTap={() => setScrollTop(s => s - 10)}>
                                    <Text text="UP" />
                                </Button>
                            </Padding>
                            <Button onTap={() => setScrollTop(s => s + 10)}>
                                <Text text="DOWN" />
                            </Button>
                        </Panel>
                    </Padding>
                    <Sized
                        sizeConstraints={{
                            width: Value.abs(200),
                            height: Value.abs(200)
                        }}
                    >
                        <Stack>
                            <Background color={Color.LIGHTGREY} />
                            <Scrollable contentWrapper={ContentWrapper}>
                                {range(1, 20).map(i => (
                                    <ListItem>Item {i}</ListItem>
                                ))}
                            </Scrollable>
                        </Stack>
                    </Sized>
                </Flex>
                <Sized
                    sizeConstraints={{
                        width: Value.abs(200),
                        height: Value.abs(200)
                    }}
                >
                    <Stack>
                        <Background color={Color.LIGHTGREY} />
                        <Scrolled scrollTop={scrollTop}>
                            {range(1, 20).map(i => (
                                <ListItem>Item {i}</ListItem>
                            ))}
                        </Scrolled>
                    </Stack>
                </Sized>
            </Flex>
        </Padding>
    )
}

export default ScrollExample
