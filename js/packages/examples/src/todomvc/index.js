window.console = {
    log,
    error: (...args) => log('ERROR: ', ...args),
    warn: (...args) => log('WARNING: ', ...args)
}

import React, { useState, createContext, useContext, useCallback } from 'react'
import { Color, Insets, Value, BoxBorders, BorderSide,BoxRadiuses, Radius } from '@advk/common'
import ReactAardvark, {
    Background,
    Border,
    Center,
    Flex,
    FlexChild,
    Sized,
    Stack,
    Text,
    Padded,
    Paragraph,
    TextSpanC,
    IntrinsicHeight,
    GestureResponder,
    Placeholder
} from '@advk/react-renderer'
import { decorate, observable, computed } from "mobx"
import { observer } from 'mobx-react'

const win = application.createWindow({ width: 640, height: 480 })
const document = application.getDocument(win)

const INITIAL_TODOS = [
    { text: 'test', isCompleted: false },
    { text: 'React is a JavaScript library for creating user interfaces. Its core principles are declarative code, efficiency, and flexibility.', isCompleted: true }
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
        return this.items.filter(item => this.filter === 'COMPLETED' ? item.isCompleted : !item.isCompleted)
    }
}

decorate(TodoStore, {
    items: observable,
    itemsLeft: computed,
    filter: observable,
    displayedItems: computed
})

const TodoStoreContext = React.createContext()

const useTodoStore = () => useContext(TodoStoreContext)

const Todo = observer(({ store }) => {
    const { text, isCompleted } = store
    const color = isCompleted ? Color.rgb(93,194,175) : Color.rgb(222,222,222)
    return (
        <Border borders={BoxBorders.only('bottom', BorderSide(1, Color.rgb(237,237,237)))}>
            <Padded padding={Insets.all(15)}>
                <Flex direction={FlexDirection.row} align={FlexAlign.center}>
                    <Padded padding={Insets.only('right', 15)}>
                        <GestureResponder
                            onTap={() => { store.isCompleted = !store.isCompleted }}
                        >
                            <Border
                                borders={BoxBorders.all(BorderSide(1, color))}
                                radiuses={BoxRadiuses.all(Radius.circular(15))}
                            >
                                <Sized sizeConstraints={{ width: Value.abs(30), height: Value.abs(30) }}>
                                    <Placeholder />
                                </Sized>
                             </Border>
                        </GestureResponder>
                    </Padded>
                    <FlexChild flex={1}>
                        <Paragraph>
                            <TextSpanC text={text} />
                        </Paragraph>
                    </FlexChild>
                </Flex>
            </Padded>
        </Border>
    )
})

const Button = ({ text, isActive, onTap }) => {
    const [isHovered, setIsHovered] = useState(false)
    const alpha = isActive ? 0.5 : (isHovered ? 0.2 : 0)
    const color = Color.rgba(175,47,47,Math.round(alpha*255))
    return (
        <GestureResponder
            onHoverStart={useCallback(() => setIsHovered(true))}
            onHoverEnd={useCallback(() => setIsHovered(false))}
            onTap={onTap}
        >
            <Border
                borders={BoxBorders.all(BorderSide(1, color))}
                radiuses={BoxRadiuses.all(Radius.circular(3))}
            >
                <Padded padding={Insets.symmetrical(7, 3)}>
                    <Text text={text} />
                </Padded>
            </Border>
        </GestureResponder>
    )
}

const TodoListFooter = observer(() => {
    const todoStore = useTodoStore()
    return (
        <Sized sizeConstraints={{ height: Value.abs(40) }}>
            <Padded padding={Insets.horiz(15)}>
                <Stack loosenConstraints={false}>
                    <Flex direction={FlexDirection.row} align={FlexAlign.center} justify={FlexJustify.spaceBetween}>
                        <Text text={`${todoStore.itemsLeft} items left`} />
                        <Text text={"Clear completed"} />
                    </Flex>
                    <Flex align={FlexAlign.center} justify={FlexJustify.center}>
                        <Padded padding={Insets.only('right', 10)}>
                            <Button text="All" onTap={() => { todoStore.filter = 'ALL' }} />
                        </Padded>
                        <Padded padding={Insets.only('right', 10)}>
                            <Button text="Active" onTap={() => { todoStore.filter = 'ACTIVE' }}/>
                        </Padded>
                        <Button text="Completed" onTap={() => { todoStore.filter = 'COMPLETED' }}/>
                    </Flex>
                </Stack>
            </Padded>
        </Sized>
    )
})

const TodoList = observer(() => {
    const todoStore = useTodoStore()
    return (
        <Background color={Color.white}>
            <IntrinsicHeight>
                <Flex direction={FlexDirection.column}>
                    {todoStore.displayedItems.map(store => <Todo store={store} />)}
                    <TodoListFooter />
                </Flex>
            </IntrinsicHeight>
         </Background>
    )
})

const store = new TodoStore()

const Main = () => {
    return (
        <TodoStoreContext.Provider value={store}>
            <Stack>
                <Background color={Color.rgb(245,245,245)} />
                <Flex justify={FlexJustify.center}>
                    <Sized sizeConstraints={{ width: Value.abs(550) }}>
                        <Flex direction={FlexDirection.column}>
                            <Padded padding={{ top: 30, bottom: 15, left: 0, right: 0 }}>
                                <Flex justify={FlexJustify.center}>
                                    <Text text="TODOS" />
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
