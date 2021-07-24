import React, { useState } from 'react'
import { Color } from '@advk/common'
import ReactAardvark, { Stack, Background } from '@advk/react-renderer'
// import ReactAardvark from '@advk/react-renderer/src/RendererApi'
// import WebApiWebSocket from '@advk/common/src/EventEmitter'

const win = application.createWindow({
    size: { width: 640, height: 640 }
})

const document = application.getDocument(win)

const Main = () => {
    return (
        <Stack>
            <Background color={Color.RED} />
        </Stack>
    )
}

ReactAardvark.render(<Main />, document)
