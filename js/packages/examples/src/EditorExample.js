import React, { useState, forwardRef } from 'react'
import {
    BoxBorders,
    BorderSide,
    Color,
    Padding as Paddings,
    Value
} from '@advk/common'
import Editor from '@advk/react-renderer/src/editor'
import { Size, Background, Padding, Flex, Border } from '@advk/react-renderer'

const Container = forwardRef(({ children, color }, ref) => (
    <Padding ref={ref} padding={Paddings.vert(10)}>
        <Border borders={BoxBorders.all(BorderSide(2, Color[color]))}>
            <Padding padding={Paddings.all(10)}>
                <Flex>{children}</Flex>
            </Padding>
        </Border>
    </Padding>
))

const Box = forwardRef(({ color }, ref) => (
    <Padding ref={ref} padding={Paddings.only('right', 10)}>
        <Size sizeConstraints={{ width: Value.abs(40), height: Value.abs(40) }}>
            <Background color={Color[color]} />
        </Size>
    </Padding>
))

const isVoid = node => node.type === 'block'

const renderElement = ({ node, children, ref }) => (
    <Container ref={ref} color={node.color}>
        {children}
    </Container>
)

const renderLeaf = ({ node, ref }) => <Box ref={ref} color={node.color} />

const initialState = [
    {
        type: 'container',
        color: 'red',
        children: [
            { type: 'box', color: 'red', text: '' },
            { type: 'box', color: 'green', text: '' },
            { type: 'box', color: 'red', text: '' }
        ]
    },
    {
        type: 'container',
        color: 'red',
        children: [{ type: 'box', color: 'green', text: '' }]
    },
    {
        type: 'container',
        color: 'blue',
        children: [
            { type: 'box', color: 'green', text: '' },
            { type: 'box', color: 'red', text: '' }
        ]
    }
]

const selection = {
    anchor: {
        path: [0, 1],
        offset: 0
    },
    focus: {
        path: [2, 0],
        offset: 0
    }
}

const caret = {}

const EditorExample = () => {
    let [state, setState] = useState(initialState)
    return (
        <Padding padding={Paddings.horiz(20)}>
            <Editor
                editorProps={{
                    isVoid,
                    selection
                }}
                state={state}
                onChange={setState}
                renderElement={renderElement}
                renderLeaf={renderLeaf}
            />
        </Padding>
    )
}

export default EditorExample
