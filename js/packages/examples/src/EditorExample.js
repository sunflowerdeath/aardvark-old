import React, { useState, forwardRef } from 'react'
import { BoxBorders, BorderSide, Color, Insets, Value } from '@advk/common'
import Editor from '@advk/react-renderer/src/components/editor'
import {
    Sized,
    Background,
    Padded,
    Flex,
    Border,
    IntrinsicWidth
} from '@advk/react-renderer'

const Container = forwardRef(({ children, color }, ref) => (
    <Padded padding={Insets.vert(10)}>
        <Border ref={ref} borders={BoxBorders.all(BorderSide(2, Color[color]))}>
            <Padded padding={Insets.all(10)}>
                <IntrinsicWidth>
                    <Flex>{children}</Flex>
                </IntrinsicWidth>
            </Padded>
        </Border>
    </Padded>
))

const Box = forwardRef(({ color, isLast }, ref) => (
    <Padded padding={isLast ? Insets.none : Insets.only('right', 10)}>
        <Sized
            ref={ref}
            sizeConstraints={{ width: Value.abs(40), height: Value.abs(40) }}
        >
            <Background color={Color[color]} />
        </Sized>
    </Padded>
))

const isVoid = node => node.type === 'box'

const renderElement = ({ node, children, ref }) => (
    <Container ref={ref} color={node.color}>
        {children}
    </Container>
)

const renderLeaf = ({ node, isLast, ref }) => (
    <Box ref={ref} isLast={isLast} color={node.color} />
)

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

const initialSelection = {
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
    let [selection, setSelection] = useState(initialSelection)
    return (
        <Padded padding={Insets.horiz(20)}>
            <Editor
                editorProps={{ isVoid }}
                state={state}
                onChange={setState}
                selection={selection}
                onChangeSelection={setSelection}
                renderElement={renderElement}
                renderLeaf={renderLeaf}
                isEditable={true}
            />
        </Padded>
    )
}

export default EditorExample
