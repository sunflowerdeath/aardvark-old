import React, {
    forwardRef,
    useRef,
    useState,
    createContext,
    useContext,
    useLayoutEffect,
    useMemo,
    useImperativeHandle,
    useCallback,
    useEffect
} from 'react'
import {
    createEditor,
    Editor,
    Element,
    Text,
    Range,
    Path,
    Transforms
} from 'slate'
import { Value, Color, Insets } from '@advk/common'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import TapRecognizer from '@advk/common/src/gestures/TapRecognizer.js'
import {
    Layer,
    Stack,
    Flex,
    Sized,
    Padded,
    Translated,
    Background,
    Responder
} from '../../nativeComponents.js'
import GestureResponder from '../GestureResponder'
import Ear from './Ear.js'
import { default as myUseContext } from '../../hooks/useContext.js'

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

class Timer {
    start(interval, callback) {
        this.timeout = setTimeout(() => {
            callback()
            this.start(interval, callback)
        }, interval)
    }

    stop() {
        clearTimeout(this.timeout)
    }
}

const Cursor = props => {
    const { ctx, blinkInterval, color } = props
    const [opacity, setOpacity] = useState(1)
    useEffect(() => {
        const timer = new Timer().start(blinkInterval, () =>
            setOpacity(val => (val === 1 ? 0 : 1))
        )
        return () => timer.stop()
    }, [])
    const node = Editor.node(ctx.editor, ctx.editor.selection.anchor)[0]
    const elem = NODES_ELEMENTS.get(node)
    const editorPos = ctx.elemRef.current.absPosition
    const { left, top } = elem.absPosition
    const { width, height } = elem.size
    const translate = {
        left: Value.abs(-editorPos.left + left),
        top: Value.abs(-editorPos.top + top)
    }
    const size = { width: Value.abs(2), height: Value.abs(height) }
    return (
        <Layer opacity={opacity}>
            <Translated translation={translate}>
                <Sized sizeConstraints={size}>
                    <Background color={color} />
                </Sized>
            </Translated>
        </Layer>
    )
}

Cursor.defaultProps = {
    blinkInterval: 750,
    color: Color.black
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
                const ears = []
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
                        <Translated translation={translate}>
                            <Sized sizeConstraints={size}>
                                <Background color={color} />
                            </Sized>
                        </Translated>
                    )

                    let isFirst = i === 0
                    let isLast = i === sortedNodes.length - 1
                    if (isFirst || isLast) {
                        const earProps = {
                            left: -absPos.left + left - padding,
                            top: -absPos.top + top - padding,
                            width: width + padding * 2,
                            height: height + padding * 2
                        }
                        if (isFirst) {
                            ears.push(<Ear side="start" {...earProps} />)
                        }
                        if (isLast) {
                            ears.push(<Ear side="end" {...earProps} />)
                        }
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

const elemContainsPoint = (elem, point) => {
    const { left, top } = elem.absPosition
    const { width, height } = elem.size
    return (
        top < point.top &&
        point.top < top + height &&
        left < point.left &&
        point.left < left + width
    )
}

const calcVertDistance = (elem, point) => {
    let above = point.top - elem.absPosition.top
    let below = point.top - elem.absPosition.top - elem.size.height
    if (above > 0 && below < 0) return 0
    // return Math.abs(above
    return Math.min(Math.abs(above), Math.abs(below))
}

const calcHorizDistance = (elem, point) => {
    let left = point.left - elem.absPosition.left
    let right = point.left - elem.absPosition.left - elem.size.width
    if (left > 0 && right < 0) return 0
    return Math.abs(left)
    // return Math.min(Math.abs(left), Math.abs(right))
}

const getNodeCursorPos = (ctx, event, node) => {
    if (Text.isText(node)) return [0]

    let minVertDistance = Infinity
    let closestNodes = []
    for (let i = 0; i < node.children.length; i++) {
        const childNode = node.children[i]
        const elem = NODES_ELEMENTS.get(childNode)
        if (elemContainsPoint(elem, event)) {
            return [i, ...getNodeCursorPos(ctx, event, childNode)]
        }
        const vertDistance = calcVertDistance(elem, event)
        if (vertDistance === minVertDistance) {
            closestNodes.push({ node, elem, index: i })
        } else if (vertDistance < minVertDistance) {
            minVertDistance = vertDistance
            closestNodes = [{ node, elem, index: i }]
        }
    }

    let minHorizDistance = Infinity
    let closestNode
    for (let i = 0; i < closestNodes.length; i++) {
        const { elem, node } = closestNodes[i]
        const horizDistance = calcHorizDistance(elem, event)
        if (horizDistance < minHorizDistance) {
            minHorizDistance = horizDistance
            closestNode = closestNodes[i]
        }
    }

    return [
        closestNode.index,
        ...getNodeCursorPos(
            ctx,
            event,
            closestNode.node.children[closestNode.index]
        )
    ]
}

const getCursorPosition = (ctx, event) => {
    const pos = getNodeCursorPos(ctx, event, ctx.editor)
    return { path: pos.slice(0, -1), offset: pos[pos.length - 1] }
}

const CursorType = { range: 0, cursor: 1 }

const onTap = (ctx, event) => {
    log('tap', JSON.stringify(event))
    if (!ctx.props.isEditable) {
        Transforms.deselect(ctx.editor)
        ctx.props.onChangeSelection(ctx.editor.selection)
    } else {
        const n = getCursorPosition(ctx, event)
        log(JSON.stringify(n))
        Transforms.setSelection(ctx.editor, { anchor: n, focus: n })
        ctx.setState({ cursorType: CursorType.cursor })
        ctx.props.onChangeSelection(ctx.editor.selection)
    }
}

const EditorComponent = forwardRef((props, ref) => {
    const {
        isSelectable,
        isEditable,
        state,
        onChange,
        selection,
        onChangeSelection,
        editorProps,
        renderElement,
        renderLeaf
    } = props

    const elemRef = useRef()
    const selectionRef = useRef()
    const ctx = myUseContext({
        props,
        initialCtx: () => {
            let editor = createEditor()
            editor.onChange = () => {
                onChange(editor.children)
                onChangeSelection(editor.selection)
            }
            return {
                elemRef,
                selectionRef,
                editor
            }
        },
        initialState: ctx => ({
            cursorType: CursorType.range,
            recognizer: new MultiRecognizer({
                tap: new TapRecognizer({
                    document: () => elemRef.current.document,
                    onTap: onTap(ctx, ?)
                })
            })
        })
    })
    useMemo(() => {
        ctx.editor.children = state
    }, [state])
    useMemo(() => {
        ctx.editor.selection = selection
    }, [selection])
    useMemo(() => {
        Object.assign(ctx.editor, editorProps)
    }, [editorProps])

    useLayoutEffect(() => {
        if (elemRef.current) {
            NODES_ELEMENTS.set(ctx.editor, elemRef.current)
            ELEMENTS_NODES.set(elemRef.current, ctx.editor)
        } else {
            NODES_ELEMENTS.delete(ctx.editor)
        }
        if (selectionRef.current != null) selectionRef.current.update()
    })

    useImperativeHandle(ref, () => ({
        editor: ctx.editor,
        insertText: text => ctx.editor.insertText(text)
    }))

    log('RENDER')

    return (
        <Responder handler={useCallback(ctx.state.recognizer.getHandler())}>
            <Stack>
                <Padded padding={Insets.all(10)} ref={elemRef}>
                    <Flex direction={FlexDirection.column}>
                        <EditorContext.Provider value={ctx.editor}>
                            <Children
                                node={ctx.editor}
                                selection={ctx.editor.selection}
                                renderElement={renderElement}
                                renderLeaf={renderLeaf}
                            />
                        </EditorContext.Provider>
                    </Flex>
                </Padded>
                {/*
                {ctx.state.cursorType === CursorType.range ? (
                    <Selection
                        ref={selectionRef}
                        editorRef={elemRef}
                        color={{ red: 152, green: 187, blue: 224, alpha: 255 }}
                        padding={4}
                        opacity={0.5}
                    />
                ) : (
                    <Cursor ctx={ctx} />
                )}
                */}
            </Stack>
        </Responder>
    )
})

export default EditorComponent
