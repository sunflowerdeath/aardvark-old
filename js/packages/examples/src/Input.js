import React, { ForwardRef, useState, useRef, useEffect } from 'react'
import { Color, Value } from '@advk/common'
import KeyCode from '@advk/common/src/events/KeyCode.js'
import ReactAardvark, {
    GestureResponder,
    Sized,
    Background,
    Flex,
    Text
} from '@advk/react-renderer'
import useCombinedRefs from '@advk/react-renderer/src/hooks/useCombinedRefs.js'
import Editor from '@advk/react-renderer/src/components/editor'

const FocusManager = {
    focus(elem, onBlur) {
        if (this.focusedElement !== undefined) this.blurElement()
        this.focusedElement = elem
        this.onBlur = onBlur
    },

    blur(elem) {
        if (this.focusedElement === elem) this.blurElement()
    },

    blurElement() {
        this.focusedElement = null
        if (this.onBlur) this.onBlur()
    }
}

const Focusable = React.forwardRef(
    ({ isFocused, onChangeFocused, children }, ref) => {
        const innerRef = useRef()
        useEffect(() => {
            return () => FocusManager.blur(innerRef.current)
        }, [])
        return (
            <GestureResponder
                ref={useCombinedRefs(ref, innerRef)}
                onPressStart={() => {
                    let elem = innerRef.current
                    let conn = elem.document.addPointerEventHandler(event => {
                        if (
                            event.action === PointerAction.pointerDown ||
                            event.action === PointerAction.buttonPress
                        ) {
                            FocusManager.blur(elem)
                        }
                    })
                    FocusManager.focus(elem, () => {
                        conn.disconnect()
                        onChangeFocused(false)
                    })
                    onChangeFocused(true)
                }}
            >
                {children}
            </GestureResponder>
        )
    }
)

const renderElement = ({ node, children, ref }) => (
    <Flex ref={ref}>{children}</Flex>
)

const renderLeaf = ({ node, isLast, ref }) => (
    <Text ref={ref} isLast={isLast} text={node.text} />
)

const initialSelection = {
    anchor: { path: [0, 0], offset: 0 },
    focus: { path: [0, 0], offset: 0 }
}

const Input = ({ value, onChange }) => {
    const [isFocused, setIsFocused] = useState(false)
    const ref = useRef()
    const editorRef = useRef()
    const conn = useRef()
    let [state, setState] = useState([{ children: [{ text: value }] }])
    let [selection, setSelection] = useState(initialSelection)

    useEffect(() => {
        if (isFocused) {
            let elem = ref.current
            conn.current = {
                char: elem.document.addCharEventHandler(event => {
                    editorRef.current.insertText(
                        String.fromCodePoint(event.codepoint)
                    )
                    // setText(
                    // text => text + String.fromCodePoint(event.codepoint)
                    // )
                }),
                key: elem.document.addKeyEventHandler(event => {
                    if (
                        event.action !== KeyAction.press &&
                        event.action !== KeyAction.repeat
                    ) {
                        return
                    }
                    if (event.key === KeyCode.BACKSPACE) {
                        editorRef.current.editor.deleteBackward('character')
                    } else if (event.key === KeyCode.DELETE) {
                        editorRef.current.editor.deleteForward('character')
                    }
                    // log(JSON.stringify(event))
                })
            }
        } else {
            if (conn.current) {
                conn.current.key.disconnect()
                conn.current.char.disconnect()
                conn.current = undefined
            }
        }
    }, [isFocused])
    useEffect(() => {
        return () => {
            if (conn.current) {
                conn.current.key.disconnect()
                conn.current.char.disconnect()
                conn.current = undefined
            }
        }
    }, [])
    return (
        <Focusable
            isFocused={isFocused}
            onChangeFocused={setIsFocused}
            ref={ref}
        >
            <Background color={isFocused ? Color.blue : Color.lightGrey}>
                <Sized
                    sizeConstraints={{
                        height: Value.abs(50),
                        width: Value.abs(100)
                    }}
                >
                    <Editor
                        ref={editorRef}
                        state={state}
                        onChange={setState}
                        selection={selection}
                        onChangeSelection={setSelection}
                        renderElement={renderElement}
                        renderLeaf={renderLeaf}
                        isEditable={true}
                    />
                </Sized>
            </Background>
        </Focusable>
    )
}

export default Input
