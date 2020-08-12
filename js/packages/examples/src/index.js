import React, { useState } from 'react'
import { Color, Insets, Value } from '@advk/common'
import ReactAardvark, {
    Background,
    Flex,
    FlexChild,
    Stack,
    Text,
    Padded,
    Size,
    Placeholder
} from '@advk/react-renderer'

import ScrollExample from './ScrollExample.js'
import LayerExample from './LayerExample.js'
import ObserverExample from './ObserverExample.js'
import EditorExample from './EditorExample.js'
import ParagraphExample from './ParagraphExample.js'
import InputExample from './InputExample.js'
import ImageExample from './ImageExample.js'
import SvgExample from './SvgExample.js'
import ShadowExample from './ShadowExample.js'
import BenchmarkExample from './BenchmarkExample.js'

import Button from './Button.js'

const win = application.createWindow({
    size: { width: 640, height: 640 }
})
const document = application.getDocument(win)

const stop = () => {
    application.stop()
}

const examples = [
    { name: 'Scroll', component: ScrollExample },
    { name: 'Layer', component: LayerExample },
    { name: 'Observer', component: ObserverExample },
    { name: 'Editor', component: EditorExample },
    { name: 'Paragraph', component: ParagraphExample },
    { name: 'Input', component: InputExample },
    { name: 'Image', component: ImageExample },
    { name: 'Svg', component: SvgExample },
    { name: 'Shadow', component: ShadowExample },
    { name: 'Benchmark', component: BenchmarkExample }
]

const popup = (event) => {
    log(JSON.stringify(event))
    application.createWindow({
        position: { left: win.position.left + event.left, top: win.position.top + event.top },
        size: { width: 150, height: 300 },
        floating: true,
        decorated: false,
        resizable: false
    })
}

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
                <Flex direction={FlexDirection.column}>
                    {examplesButtons}
                    <Button onTap={stop}>
                        <Text text="STOP" />
                    </Button>
                    <Button onTap={gc}>
                        <Text text="GC" />
                    </Button>
                    <Button onTap={popup}>
                        <Text text="popup" />
                    </Button>
                </Flex>
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
                        <Padded padding={Insets.only('left', 16)}>
                            <Text text={selected.name} />
                        </Padded>
                    </FlexChild>
                </Flex>
                {React.createElement(selected.component)}
            </Flex>
        </Stack>
    )
}

ReactAardvark.render(<Main />, document)
