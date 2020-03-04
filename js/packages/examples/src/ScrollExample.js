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
    TransformMatrix
} from '@advk/common'
import ReactAardvark, {
    Align,
    GestureResponder,
    Size,
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
import FloatingScrollbarWrapper from '@advk/react-renderer/src/components/FloatingScrollbarWrapper'
import Button from './Button.js'

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
        <Size sizeConstraints={{ height: Value.abs(isExpanded ? 150 : 50) }}>
            <Background color={Color.PURPLE} />
        </Size>
    )
})

const BasicScrollExample = () => {
    const expandRef = useRef()
    return (
        <>
            <Flex>
                <Button onTap={() => expandRef.current.toggle()}>
                    <Text text="Expand" />
                </Button>
                <Button onTap={() => expandRef.current.toggle()}>
                    <Text text="Disable" />
                </Button>
            </Flex>
            <Size
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
            </Size>
        </>
    )
}

const FloatingScrollbarExample = () => {
    const expandRef = useRef()
    return (
        <>
            <Button onTap={() => expandRef.current.toggle()}>
                <Text text="Expand" />
            </Button>
            <Size
                sizeConstraints={{
                    width: Value.abs(200),
                    height: Value.abs(200)
                }}
            >
                <Stack>
                    <Background color={Color.LIGHTGREY} />
                    <Scrollable wrapper={FloatingScrollbarWrapper}>
                        {range(1, 20).map(i => (
                            <ListItem>Item {i}</ListItem>
                        ))}
                    </Scrollable>
                </Stack>
            </Size>
        </>
    )
}

const examples = [
    { name: 'Basic', component: BasicScrollExample },
    { name: 'Floating scrollbar', component: FloatingScrollbarExample }
]

const ScrollExample = () => {
    const [selected, setSelected] = useState()
    return (
        <Padding padding={Padding1.all(16)}>
            <Flex direction={FlexDirection.column}>
                <Padding padding={Padding1.only('bottom', 16)}>
                    <Flex>
                        {examples.map(item => (
                            <Padding padding={Padding1.only('right', 8)}>
                                <Button onTap={() => setSelected(item)}>
                                    <Text text={item.name} />
                                </Button>
                            </Padding>
                        ))}
                    </Flex>
                </Padding>
                {selected ? React.createElement(selected.component) : null}
            </Flex>
        </Padding>
    )
}

export default ScrollExample
