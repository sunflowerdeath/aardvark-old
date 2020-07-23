window.console = {
    log,
    error: (...args) => log('ERROR: ', ...args),
    warn: (...args) => log('WARNING: ', ...args)
}

import React, { useState, createContext, useContext, useCallback } from 'react'
import {
    Alignment,
    Color,
    Insets,
    Value,
    BoxBorders,
    BorderSide,
    BoxRadiuses,
    Radius
} from '@advk/common'
import ReactAardvark, {
    Aligned,
    Background,
    Border,
    Center,
    Container,
    Flex,
    FlexChild,
    Image,
    Sized,
    Stack,
    StackChild,
    Text,
    Padded,
    Paragraph,
    Translated,
    TextSpanC,
    IntrinsicHeight,
    GestureResponder,
    Placeholder
} from '@advk/react-renderer'
import { decorate, observable, computed, action } from 'mobx'
import { observer } from 'mobx-react'

const win = application.createWindow({ width: 640, height: 640 })
const document = application.getDocument(win)

const INITIAL_TODOS = [
    { text: 'test', isCompleted: false },
    {
        text:
            'React is a JavaScript library for creating user interfaces. Its core principles are declarative code, efficiency, and flexibility.',
        isCompleted: true
    }
]

class TodoStore {
    constructor() {
        this.items = INITIAL_TODOS
        this.filter = 'ALL'
    }

    get itemsLeft() {
        let res = 0
        this.items.forEach(todo => {
            if (!todo.isCompleted) res++
        })
        return res
    }

    get displayedItems() {
        if (this.filter === 'ALL') return this.items
        return this.items.filter(item =>
            this.filter === 'COMPLETED' ? item.isCompleted : !item.isCompleted
        )
    }

    clearCompleted() {
        this.items = this.items.filter(item => !item.isCompleted)
    }

    toggleAll() {
        const nextIsCompleted = this.itemsLeft > 0
        this.items.forEach(item => {
            item.isCompleted = nextIsCompleted
        })
    }
}

decorate(TodoStore, {
    items: observable,
    itemsLeft: computed,
    filter: observable,
    displayedItems: computed,
    toggleAll: action
})

const TodoStoreContext = React.createContext()

const useTodoStore = () => useContext(TodoStoreContext)

const borderColor = Color.rgb(237, 237, 237)

const footerTextStyle = { fontSize: 14, color: Color.rgb(119, 119, 119) }

const Todo = observer(({ store }) => {
    const todoStore = useTodoStore()
    const [isHovered, setIsHovered] = useState(false)
    const { text, isCompleted } = store
    const color = isCompleted
        ? Color.rgb(93, 194, 175)
        : Color.rgb(222, 222, 222)

    let row = (
        <Padded padding={Insets.all(15)}>
            <Flex direction={FlexDirection.row} align={FlexAlign.center}>
                <Padded padding={Insets.only('right', 15)}>
                    <GestureResponder
                        onTap={() => {
                            store.isCompleted = !store.isCompleted
                        }}
                    >
                        <Container
                            sizeConstraints={{
                                width: Value.abs(30),
                                height: Value.abs(30)
                            }}
                            borders={BoxBorders.all(BorderSide(2, color))}
                            radiuses={BoxRadiuses.all(Radius.circular(15))}
                        >
                            <Placeholder />
                        </Container>
                    </GestureResponder>
                </Padded>
                <FlexChild flex={1}>
                    <Paragraph>
                        <TextSpanC text={text} style={{ fontSize: 24 }} />
                    </Paragraph>
                </FlexChild>
            </Flex>
        </Padded>
    )

    let removeButton
    if (isHovered) {
        removeButton = (
            <StackChild floating={true}>
                <Aligned
                    alignment={Alignment.topRight(
                        Value.rel(0.5),
                        Value.abs(10)
                    )}
                >
                    <Translated
                        translation={{ top: Value.rel(-0.5), left: Value.none }}
                    >
                        <GestureResponder
                            onTap={() => {
                                todoStore.items.remove(store)
                            }}
                        >
                            <Container
                                margin={Insets.all(10)}
                                sizeConstraints={{
                                    width: Value.abs(15),
                                    height: Value.abs(15)
                                }}
                                background={Color.red}
                            />
                        </GestureResponder>
                    </Translated>
                </Aligned>
            </StackChild>
        )
    }

    return (
        <GestureResponder
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
        >
            <Border
                borders={BoxBorders.only('bottom', BorderSide(1, borderColor))}
            >
                <Stack>
                    {row}
                    {removeButton}
                </Stack>
            </Border>
        </GestureResponder>
    )
})

const Button = ({ text, isActive, onTap }) => {
    const [isHovered, setIsHovered] = useState(false)
    const alpha = isActive ? 0.5 : isHovered ? 0.2 : 0
    const color = Color.rgba(175, 47, 47, Math.round(alpha * 255))
    return (
        <GestureResponder
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
            onTap={onTap}
        >
            <Container
                borders={BoxBorders.all(BorderSide(1, color))}
                radiuses={BoxRadiuses.all(Radius.circular(3))}
                padding={Insets.symmetrical(7, 3)}
            >
                <Text text={text} style={footerTextStyle} />
            </Container>
        </GestureResponder>
    )
}

const TodoListHeader = observer(() => {
    const todoStore = useTodoStore()

    let toggleAll
    if (todoStore.items.length > 0) {
        const alpha = todoStore.itemsLeft === 0 ? 1 : 0.5
        const color = Color.rgba(77, 77, 77, Math.round(alpha * 255))
        toggleAll = (
            <StackChild floating={true}>
                <Aligned
                    alignment={Alignment.topLeft(Value.rel(0.5), Value.abs(5))}
                    adjustChildSize={false}
                >
                    <Translated
                        translation={{ top: Value.rel(-0.5), left: Value.none }}
                    >
                        <GestureResponder onTap={() => todoStore.toggleAll()}>
                            <Container
                                margin={Insets.all(10)}
                                sizeConstraints={{
                                    height: Value.abs(20),
                                    width: Value.abs(30)
                                }}
                                background={color}
                            />
                        </GestureResponder>
                    </Translated>
                </Aligned>
            </StackChild>
        )
    }

    return (
        <Border borders={BoxBorders.only('bottom', BorderSide(1, borderColor))}>
            <Stack>
                <Sized sizeConstraints={{ height: Value.abs(65) }}>
                    <Flex align={FlexAlign.center}>
                        <Padded padding={Insets.only('left', 60)}>
                            <Text
                                text={'What needs to be done'}
                                style={{
                                    fontSize: 24,
                                    color: Color.rgb(230, 230, 230)
                                }}
                            />
                        </Padded>
                    </Flex>
                </Sized>
                {toggleAll}
            </Stack>
        </Border>
    )
})

const TodoListFooter = observer(() => {
    const todoStore = useTodoStore()

    let clearCompleted
    if (todoStore.itemsLeft < todoStore.items.length) {
        clearCompleted = (
            <GestureResponder onTap={() => todoStore.clearCompleted()}>
                <Text text={'Clear completed'} style={footerTextStyle} />
            </GestureResponder>
        )
    }

    return (
        <Sized sizeConstraints={{ height: Value.abs(40) }}>
            <Padded padding={Insets.horiz(15)}>
                <Stack loosenConstraints={false}>
                    <Flex
                        direction={FlexDirection.row}
                        align={FlexAlign.center}
                        justify={FlexJustify.spaceBetween}
                    >
                        <Text
                            text={`${todoStore.itemsLeft} items left`}
                            style={footerTextStyle}
                        />
                        {clearCompleted}
                    </Flex>
                    <Flex align={FlexAlign.center} justify={FlexJustify.center}>
                        <Padded padding={Insets.only('right', 10)}>
                            <Button
                                text="All"
                                isActive={todoStore.filter === 'ALL'}
                                onTap={() => {
                                    todoStore.filter = 'ALL'
                                }}
                            />
                        </Padded>
                        <Padded padding={Insets.only('right', 10)}>
                            <Button
                                text="Active"
                                isActive={todoStore.filter === 'ACTIVE'}
                                onTap={() => {
                                    todoStore.filter = 'ACTIVE'
                                }}
                            />
                        </Padded>
                        <Button
                            text="Completed"
                            isActive={todoStore.filter === 'COMPLETED'}
                            onTap={() => {
                                todoStore.filter = 'COMPLETED'
                            }}
                        />
                    </Flex>
                </Stack>
            </Padded>
        </Sized>
    )
})

const shadowColor = Color.rgba(0,0,0,Math.round(255*0.1))

const TodoList = observer(() => {
    const todoStore = useTodoStore()
    return (
        <Border
            shadows={[
                {
                    offset: { left: 0, top: 2 },
                    blur: 4,
                    color: shadowColor
                },
                {
                    offset: { left: 0, top: 25 },
                    blur: 50,
                    color: shadowColor
                }
            ]}
        >
            <Background color={Color.white}>
                <IntrinsicHeight>
                    <Flex direction={FlexDirection.column}>
                        <TodoListHeader />
                        {todoStore.displayedItems.map(store => (
                            <Todo store={store} />
                        ))}
                        <TodoListFooter />
                    </Flex>
                </IntrinsicHeight>
            </Background>
        </Border>
    )
})

const store = new TodoStore()

const Main = () => {
    return (
        <TodoStoreContext.Provider value={store}>
            <Stack>
                <Background color={Color.rgb(245, 245, 245)} />
                <Flex justify={FlexJustify.center}>
                    <Sized sizeConstraints={{ width: Value.abs(550) }}>
                        <Flex direction={FlexDirection.column}>
                            <Padded
                                padding={{
                                    top: 30,
                                    bottom: 15,
                                    left: 0,
                                    right: 0
                                }}
                            >
                                <Flex justify={FlexJustify.center}>
                                    <Text
                                        text="todos"
                                        style={{
                                            color: Color.rgb(234, 215, 215),
                                            fontSize: 100
                                        }}
                                    />
                                </Flex>
                            </Padded>
                            <TodoList />
                        </Flex>
                    </Sized>
                </Flex>
            </Stack>
        </TodoStoreContext.Provider>
    )
}

ReactAardvark.render(<Main />, document)
