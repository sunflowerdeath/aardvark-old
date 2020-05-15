import React, { useState, forwardRef } from 'react'
import {
    BoxBorders,
    BorderSide,
    Color,
    Padding as Paddings,
    Value
} from '@advk/common'
import Editor from '@advk/react-renderer/src/components/editor'
import {
    Size,
    Background,
    Padding,
    Flex,
    Border,
    IntrinsicWidth
} from '@advk/react-renderer'

const Container = forwardRef(({ children, color }, ref) => (
    <Padding padding={Paddings.vert(10)}>
        <Border ref={ref} borders={BoxBorders.all(BorderSide(2, Color[color]))}>
            <Padding padding={Paddings.all(10)}>
                <IntrinsicWidth>
                    <Flex>{children}</Flex>
                </IntrinsicWidth>
            </Padding>
        </Border>
    </Padding>
))

const Box = forwardRef(({ color, isLast }, ref) => (
    <Padding padding={isLast ? Paddings.none : Paddings.only('right', 10)}>
        <Size
            ref={ref}
            sizeConstraints={{ width: Value.abs(40), height: Value.abs(40) }}
        >
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
        <Padding padding={Paddings.horiz(20)}>
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
        </Padding>
    )
}

export default EditorExample