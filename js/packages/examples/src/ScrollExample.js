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
    Insets,
    TransformMatrix
} from '@advk/common'
import ReactAardvark, {
    GestureResponder,
    Sized,
    Stack,
    Background,
    Center,
    Clip,
    Layer,
    Padded,
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
        if (eventType === ResponderEventType.add) setIsHovered(true)
        else if (eventType === ResponderEventType.remove) setIsHovered(false)
    }
    return (
        <Responder handler={handler}>
            <IntrinsicHeight>
                <Stack>
                    <Background
                        color={isHovered ? Color.green : Color.transparent}
                    />
                    <Padded padding={Insets.all(16)}>
                        <Text text={children.join('')} />
                    </Padded>
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
        <Sized sizeConstraints={{ height: Value.abs(isExpanded ? 150 : 50) }}>
            <Background color={Color.PURPLE} />
        </Sized>
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
            <Sized
                sizeConstraints={{
                    width: Value.abs(200),
                    height: Value.abs(200)
                }}
            >
                <Stack>
                    <Background color={Color.LIGHTGREY} />
                    <Scrollable wrapper={FloatingScrollbarWrapper}>
                        <Expandable ref={expandRef} />
                        {range(1, 20).map(i => (
                            <ListItem>Item {i}</ListItem>
                        ))}
                    </Scrollable>
                </Stack>
            </Sized>
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
        <Padded padding={Insets.all(16)}>
            <Flex direction={FlexDirection.column}>
                <Padded padding={Insets.only('bottom', 16)}>
                    <Flex>
                        {examples.map(item => (
                            <Padded padding={Insets.only('right', 8)}>
                                <Button onTap={() => setSelected(item)}>
                                    <Text text={item.name} />
                                </Button>
                            </Padded>
                        ))}
                    </Flex>
                </Padded>
                {selected ? React.createElement(selected.component) : null}
            </Flex>
        </Padded>
    )
}

export default ScrollExample
