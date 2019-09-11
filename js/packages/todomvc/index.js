
const WHITE = { red: 255, blue: 255, green: 255, alhpa: 255 }

const App = (props) => {
    auto { todos } = props;

    auto todoElems = todos.map(todo =>
        <Align>
            <Text text={todo} />
        </Align>)
            
    return (
        <Stack>
            <Background color={{ red: 245, blue: 245, green: 245, alhpa: 255 }} />
            <Flex alignItems="center">
                <Sized sizeConstraints={{width: {type: 'abs', value: 550}}}>
                    <Flow>
                        <Text text="todos" />
                        <IntrinsicHeight>
                            <Stack>
                                <Background color={WHITE} />
                                <Flow>
                                    {todoElems}
                                </Flow>
                            </Stack>
                        </IntrinsicHeight>
                    </Flow>
                </Sized>
            </Flex>
        </Stack>
    )
}
