import React, {
    useRef,
    useEffect,
    useState,
    useCallback,
    useImperativeHandle
} from 'react'
import {
    Color,
    Value,
    Padding as Padding1,
    FlexDirection,
    PointerEventType,
    TransformMatrix
} from '@advk/common'
import ReactAardvark, {
    Align,
    GestureResponder,
    Sized,
    Stack,
    Background,
    Center,
    Clip,
    Layer,
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
                <Stack>
                    <Background
                        color={isHovered ? HOVERED_COLOR : INITIAL_COLOR}
                    />
                    <Padding padding={Padding1.all(16)}>
                        <Flex direction={FlexDirection.column}>{children}</Flex>
                    </Padding>
                </Stack>
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

const FloatingScrollbar = props => {
    const { children, height, scrollHeight, scrollTopValue } = props
    const scrollbarHeight = (height * height) / scrollHeight
    const layerRef = useRef()
    useEffect(() => {
        scrollTopValue.addListener(({ value }) => {
            const top = (height * value) / scrollHeight
            layerRef.current.transform = TransformMatrix.makeTranslate(0, top)
        })
    }, [height, scrollHeight])
    return (
        <Stack>
            {children}
            <Align alignment={{ top: Value.abs(0), right: Value.abs(0) }}>
                <Sized
                    sizeConstraints={{
                        height: Value.rel(1),
                        width: Value.abs(10)
                    }}
                >
                    <Clip>
                        <Layer
                            ref={layerRef}
                            transform={TransformMatrix.makeTranslate(0, 0)}
                        >
                            <Sized
                                sizeConstraints={{
                                    height: Value.abs(scrollbarHeight),
                                    width: Value.rel(1)
                                }}
                            >
                                <Background
                                    color={{
                                        red: 0,
                                        blue: 0,
                                        green: 0,
                                        alpha: 128
                                    }}
                                />
                            </Sized>
                        </Layer>
                    </Clip>
                </Sized>
            </Align>
        </Stack>
    )
}

const Expandable = React.forwardRef((props, ref) => {
    const [isExpanded, setIsExpanded] = useState(false)
    useImperativeHandle(
        ref,
        () => ({
            toggle: () => setIsExpanded(value => !value)
        }),
        []
    )
    return (
        <Sized sizeConstraints={{ height: Value.abs(isExpanded ? 150 : 50) }}>
            <Background color={Color.PURPLE} />
        </Sized>
    )
})

const ScrollExample = () => {
    const [scrollTop, setScrollTop] = useState(0)
    const expandRef = useRef()
    return (
        <Padding padding={Padding1.all(16)}>
            <Flex direction={FlexDirection.column}>
                <Padding padding={Padding1.only('bottom', 16)}>
                    <Panel>
                        <Button onTap={() => setScrollTop(s => s - 10)}>
                            <Text text="UP" />
                        </Button>
                        <Button onTap={() => setScrollTop(s => s + 10)}>
                            <Text text="DOWN" />
                        </Button>
                        <Flex>
                            <Button onTap={() => expandRef.current.toggle()}>
                                <Text text="Expand container" />
                            </Button>
                            <Button onTap={() => expandRef.current.toggle()}>
                                <Text text="Expand child" />
                            </Button>
                        </Flex>
                    </Panel>
                </Padding>
                <Flex>
                    <Sized
                        sizeConstraints={{
                            width: Value.abs(200),
                            height: Value.abs(200)
                        }}
                    >
                        <Stack>
                            <Background color={Color.LIGHTGREY} />
                            <Scrollable>
                                <Expandable ref={expandRef} />
                                {range(1, 2).map(i => (
                                    <ListItem>Item {i}</ListItem>
                                ))}
                            </Scrollable>
                        </Stack>
                    </Sized>
                    <Sized
                        sizeConstraints={{
                            width: Value.abs(200),
                            height: Value.abs(200)
                        }}
                    >
                        <Stack>
                            <Background color={Color.LIGHTGREY} />
                            <Scrollable wrapper={FloatingScrollbar}>
                                {range(1, 20).map(i => (
                                    <ListItem>Item {i}</ListItem>
                                ))}
                            </Scrollable>
                        </Stack>
                    </Sized>
                </Flex>
            </Flex>
        </Padding>
    )
}

export default ScrollExample
