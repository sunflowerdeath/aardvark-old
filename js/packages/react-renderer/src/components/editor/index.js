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
import { createEditor, Editor, Element, Range, Path } from 'slate'
import { Value, Color, Padding } from '@advk/common'
import {
    Layer,
    Stack,
    Flex,
    Size,
    Padding as Padding1,
    Translate,
    Background
} from '../../nativeComponents.js'
import GestureResponder from '../GestureResponder'
import Ear from './Ear.js'

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
        const isLast = i === node.children.length - 1
        if (Element.isElement(child)) {
            children.push(
                <ElementComponent
                    node={child}
                    selection={childSelection}
                    isFullySelected={isFullySelected}
                    isLast={isLast}
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
                    isLast={isLast}
                />
            )
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
        isLast,
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

    return renderElement({ node, ref, isLast, children })
}

const LeafComponent = props => {
    const { node, parent, isLast, renderLeaf } = props
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

    return renderLeaf({ node, ref, isLast })
}

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
                const size = SELECTED_NODES.size
                let sortedNodes = Array.from(SELECTED_NODES)
                    .map(node => ({
                        node,
                        path: findPath(node)
                    }))
                    .sort((a, b) => Path.compare(a.path, b.path))
                for (let i = 0; i < sortedNodes.length; i++) {
                    let node = sortedNodes[i].node
                    const elem = NODES_ELEMENTS.get(node)
                    const { left, top } = elem.absPosition
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

                    let isFirst = i === 0
                    let isLast = i === sortedNodes.length - 1
                    if (isFirst || isLast) {
                        ears.push(
                            <Ear
                                side={isLast ? 'end' : 'start'}
                                left={-absPos.left + left - padding}
                                top={-absPos.top + top - padding}
                                width={width + padding * 2}
                                height={height + padding * 2}
                            />
                        )
                    }
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
            <Layer>
                <Stack>{ears}</Stack>
            </Layer>
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
            <Padding1 padding={Padding.all(10)} ref={elemRef}>
                <Flex direction={FlexDirection.column}>
                    <EditorContext.Provider value={editor}>
                        <Children
                            node={editor}
                            selection={editor.selection}
                            renderElement={renderElement}
                            renderLeaf={renderLeaf}
                        />
                    </EditorContext.Provider>
                </Flex>
            </Padding1>
            <Selection
                ref={selectionRef}
                editorRef={elemRef}
                color={{ red: 152, green: 187, blue: 224, alpha: 255 }}
                padding={4}
                opacity={0.5}
            />
        </Stack>
    )
}

export default EditorComponent
