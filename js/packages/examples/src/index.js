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

import Button from './Button.js'

const win = application.createWindow({ width: 640, height: 480 })
const document = application.getDocument(win)

const stop = () => {
    // ReactAardvark.render(<Placeholder />, document)
    application.stop()
}

/*
import { useRef, useEffect, useCallback } from 'react'
import { Responder } from '@advk/react-renderer'
import useLastValue from '@advk/react-renderer/src/hooks/useLastValue'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'

const Govno = (props) => {
    const [handler] = useState(() => {
        return function C() { props.cb() }
    })
    // const didUnmountRef = useRef(false)
    // useEffect(() => {
        // return function A() {
            // log('UNMOUNT')
            // didUnmountRef.current = true
        // }
    // }, [])
    return <Responder handler={handler}><Placeholder/></Responder>
}

const Main = () => {
   // return (
        // <Button>
            // <Background color={Color.red} />
        // </Button>
    // )
   const [s, setS] = useState(false)
   return <Govno cb={function B() { setS(true) }}/>
}

ReactAardvark.render(<Main/>, document)

setTimeout(stop, 500)
*/

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
                <Flex direction={FlexDirection.column}>
                    {examplesButtons}
                    <Button onTap={stop}>
                        <Text text="STOP" />
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
