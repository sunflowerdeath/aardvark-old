import React, { useState, useEffect } from '@advk/react-renderer/src/react'
import { Color } from '@advk/common'
import ReactAardvark, { Stack, Background } from '@advk/react-renderer'
// import ReactAardvark from '@advk/react-renderer/src/RendererApi'
// import WebApiWebSocket from '@advk/common/src/EventEmitter'

const win = application.createWindow({
    size: { width: 640, height: 480 }
})

const document = application.getDocument(win)

/*
const bg = new BackgroundElement()
bg.color = Color.RED

const stack = new StackElement()
stack.appendChild(bg)

document.root = stack

setTimeout(() => {
  bg.color = Color.GREEN
}, 1000)
*/

const Main = () => {
    let [color, setColor] = useState(Color.RED)
    useEffect(() => {
      setTimeout(() => setColor(Color.GREEN), 2000)
    }, [])
    return (
        <Stack>
            <Background color={color} />
        </Stack>
    )
}

ReactAardvark.render(<Main />, document)
