import React, { useState } from 'react'
import { Color, Padding as Padding1, Value } from '@advk/common'
import ReactAardvark, {
    Background,
    Flex,
    FlexChild,
    Stack,
    Text,
    Padding,
    Size
} from '@advk/react-renderer'

import ScrollExample from './ScrollExample.js'
import LayerExample from './LayerExample.js'
import ObserverExample from './ObserverExample.js'
import EditorExample from './EditorExample.js'
import ParagraphExample from './ParagraphExample.js'

import Button from './Button.js'

const examples = [
    { name: 'Scroll', component: ScrollExample },
    { name: 'Layer', component: LayerExample },
    { name: 'Observer', component: ObserverExample },
    { name: 'Editor', component: EditorExample },
    { name: 'Paragraph', component: ParagraphExample }
]

const Main = () => {
    const [selected, setSelected] = useState()
    if (selected === undefined) {
        const examplesButtons = examples.map(item => (
            <Button onTap={() => setSelected(item)}>
                <Text text={item.name} />
            </Button>
        ))

        return (
            <Stack>
                <Background color={Color.WHITE} />
                <Flex direction={FlexDirection.column}>{examplesButtons}</Flex>
            </Stack>
        )
    }

    return (
        <Stack>
            <Background color={Color.WHITE} />
            <Flex direction={FlexDirection.column}>
                <Flex>
                    <Button onTap={() => setSelected(undefined)}>
                        <Text text="<- BACK" />
                    </Button>
                    <FlexChild align={FlexAlign.center}>
                        <Padding padding={Padding1.only('left', 16)}>
                            <Text text={selected.name} />
                        </Padding>
                    </FlexChild>
                </Flex>
                {React.createElement(selected.component)}
            </Flex>
        </Stack>
    )
}

const win = application.createWindow({ width: 640, height: 480 })
const document = application.getDocument(win)
ReactAardvark.render(<Main />, document)
