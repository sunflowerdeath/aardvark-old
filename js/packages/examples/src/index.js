window.console = {
    log,
    error: (...args) => log('ERROR: ', ...args),
    warn: (...args) => log('WARNING: ', ...args)
}

import React, { useState } from 'react'
import { Color, Insets, Value, BoxBorders, BorderSide } from '@advk/common'
import ReactAardvark, {
    Background,
    Border,
    Center,
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

const Popup = () => (
    <Background color={Color.rgb(45, 45, 45)}>
        <Border borders={BoxBorders.all(BorderSide(1, Color.black))}>
            <Center>
                <Text text="Popup" style={{ color: Color.white }} />
            </Center>
        </Border>
    </Background>
)

const popup = event => {
    let popupWin = application.createWindow({
        position: {
            left: win.position.left + event.left,
            top: win.position.top + event.top
        },
        size: { width: 200, height: 250 },
        floating: true,
        decorated: false,
        resizable: false,
        visible: false
    })
    let popupDoc = application.getDocument(popupWin)
    ReactAardvark.render(<Popup />, popupDoc)
    requestAnimationFrame(() => {
        popupWin.show()
        let conn = popupWin.addWindowEventHandler(event => {
            if (event.type === 'blur') {
                conn.disconnect()
                popupWin.hide()
                application.destroyWindow(popupWin)
            }
        })
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
