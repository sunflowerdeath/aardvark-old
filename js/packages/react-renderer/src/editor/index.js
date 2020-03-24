import React, {
    forwardRef,
    useRef,
    useState,
    createContext,
    useContext,
    useLayoutEffect,
    useMemo,
    useImperativeHandle
} from 'react'
import { createEditor, Editor, Element, Range } from 'slate'
import { Value, Color } from '@advk/common'
import {
    Layer,
    Stack,
    Flex,
    Translate,
    Size,
    Background
} from '../nativeComponents.js'
import GestureResponder from '../components/GestureResponder'

const EditorContext = createContext()

const useEditor = () => {
    const editor = useContext(EditorContext)
    if (!editor) {
        throw new Error('useEditor() should be used inside Editor component.')
    }
    return editor
}

// Weak maps that allow to rebuild a path given a node.
const NODES_PARENTS = new WeakMap()
const NODES_INDEXES = new WeakMap()

const NODES_ELEMENTS = new WeakMap()
const ELEMENTS_NODES = new WeakMap()

const SELECTED_NODES = new Set()

const findPath = node => {
    const path = []
    let child = node
    while (true) {
        const parent = NODES_PARENTS.get(child)
        if (parent == null) {
            if (Editor.isEditor(child)) {
                return path
            } else {
                break
            }
        }
        const i = NODES_INDEXES.get(child)
        if (i == null) break
        path.unshift(i)
        child = parent
    }
    throw new Error(
        `Unable to find the path for Slate node: ${JSON.stringify(node)}`
    )
}

const Children = props => {
    const {
        node,
        selection,
        isInsideSelected,
        renderElement,
        renderLeaf
    } = props
    const editor = useEditor()
    const path = findPath(node)

    /*
    const isLeafBlock =
        Element.isElement(node) &&
        !editor.isInline(node) &&
        Editor.hasInlines(editor, node)
    */

    const children = []
    for (let i = 0; i < node.children.length; i++) {
        const child = node.children[i]
        const childPath = path.concat(i)
        const childRange = Editor.range(editor, childPath)
        const childSelection =
            selection && Range.intersection(childRange, selection)
        const isFullySelected =
            !isInsideSelected &&
            childSelection &&
            Range.equals(childRange, childSelection)
        if (Element.isElement(child)) {
            children.push(
                <ElementComponent
                    node={child}
                    selection={childSelection}
                    isFullySelected={isFullySelected}
                    renderElement={renderElement}
                    renderLeaf={renderLeaf}
                />
            )
        } else {
            children.push(
                <LeafComponent
                    node={child}
                    parent={node}
                    selection={childSelection}
                    renderLeaf={renderLeaf}
                />
            )
            // isLast={isLeafBlock && i === node.children.length - 1}
        }

        if (isFullySelected) {
            SELECTED_NODES.add(child)
        } else {
            SELECTED_NODES.delete(child)
        }

        NODES_INDEXES.set(child, i)
        NODES_PARENTS.set(child, node)
    }

    return <>{children}</>
}

const ElementComponent = props => {
    const {
        node,
        renderElement,
        renderLeaf,
        isFullySelected,
        selection
    } = props
    const ref = useRef()
    // const editor = useEditor()

    useLayoutEffect(() => {
        if (ref.current) {
            NODES_ELEMENTS.set(node, ref.current)
            ELEMENTS_NODES.set(ref.current, node)
        } else {
            NODES_ELEMENTS.delete(node)
            // ELEMENTS_NODES.delete(ref.current, node)
        }
    })

    const children = (
        <Children
            node={node}
            selection={selection}
            isInsideSelected={isFullySelected}
            renderElement={renderElement}
            renderLeaf={renderLeaf}
        />
    )

    return renderElement({ node, ref, children })
}

const LeafComponent = props => {
    const { node, parent, renderLeaf } = props
    // const editor = useEditor()
    const ref = useRef()

    /*
    TODO decorations
    const leaves = SlateText.decorations(text, decorations)
    */
    useLayoutEffect(() => {
        if (ref.current) {
            NODES_ELEMENTS.set(node, ref.current)
            ELEMENTS_NODES.set(ref.current, node)
        } else {
            NODES_ELEMENTS.delete(node)
            // ELEMENTS_NODES.delete(ref.current, node)
        }
    })

    return renderLeaf({ node, ref })
}

const Ear = () => {}

const Selection = forwardRef((props, ref) => {
    const { editorRef, color, padding, opacity } = props
    const [children, setChildren] = useState([])
    const [ears, setEars] = useState([])
    useImperativeHandle(
        ref,
        () => ({
            update: () => {
                editorRef.current.document.partialRelayout(editorRef.current)
                const children = []
                const absPos = editorRef.current.absPosition
                // const absPos = { left: 0, top: 0 }
                for (let node of SELECTED_NODES) {
                    const elem = NODES_ELEMENTS.get(node)
                    const { left, top } = elem.absPosition
                    // const { left, top } = elem.relPosition
                    const { width, height } = elem.size
                    const translate = {
                        left: Value.abs(-absPos.left + left - padding),
                        top: Value.abs(-absPos.top + top - padding)
                    }
                    const size = {
                        width: Value.abs(width + padding * 2),
                        height: Value.abs(height + padding * 2)
                    }
                    children.push(
                        <Translate translation={translate}>
                            <Size sizeConstraints={size}>
                                <Background color={color} />
                            </Size>
                        </Translate>
                    )

                    const earSize = {
                        width: Value.abs(2),
                        height: Value.abs(height + padding * 2)
                    }
                    ears.push(
                        <Translate translation={translate}>
                            <Size sizeConstraints={earSize}>
                                <Background
                                    color={{
                                        red: 19,
                                        green: 111,
                                        blue: 225,
                                        alpha: 255
                                    }}
                                />
                            </Size>
                        </Translate>
                    )
                }
                setChildren(children)
                setEars(ears)
            }
        }),
        []
    )
    return (
        <>
            <Layer opacity={opacity}>
                <Stack>{children}</Stack>
            </Layer>
            <Stack>{ears}</Stack>
        </>
    )
})

const EditorComponent = props => {
    const { state, editorProps, renderElement, renderLeaf } = props

    const editor = useMemo(() => createEditor())
    useMemo(() => {
        Object.assign(editor, editorProps)
    }, [editorProps])
    useMemo(() => {
        editor.children = state
    }, [state])

    const elemRef = useRef()
    const selectionRef = useRef()
    useLayoutEffect(() => {
        if (elemRef.current) {
            NODES_ELEMENTS.set(editor, elemRef.current)
            ELEMENTS_NODES.set(elemRef.current, editor)
        } else {
            NODES_ELEMENTS.delete(editor)
        }
        if (selectionRef.current != null) selectionRef.current.update()
    })

    return (
        <Stack>
            <Flex direction={FlexDirection.column} ref={elemRef}>
                <EditorContext.Provider value={editor}>
                    <Children
                        node={editor}
                        selection={editor.selection}
                        renderElement={renderElement}
                        renderLeaf={renderLeaf}
                    />
                </EditorContext.Provider>
            </Flex>
            <Selection
                ref={selectionRef}
                editorRef={elemRef}
                color={{ red: 152, green: 187, blue: 224, alpha: 255 }}
                padding={3}
                opacity={0.5}
            />
        </Stack>
    )
}

export default EditorComponent
