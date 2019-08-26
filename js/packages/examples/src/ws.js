// let ws = new WebSocket()
// ws.addStartHandler(() => {
    // log('start')
    // ws.send('Hello, server')
// })
// ws.addCloseHandler(() => log('close'))
// ws.addErrorHandler(() => log('error'))
// ws.addMessageHandler(event => log('message: ' + event.data))

import WebApiWebSocket from '@advk/common/src/WebApiWebSocket'
// const ws = new WebApiWebSocket('ws://echo.websocket.org')
const ws = new WebApiWebSocket('ws://wrong')
ws.onstart = () => {
    log('start')
    ws.send('Hello!')
}
ws.onmessage = event => {
    log('message: ' + event.data)
    setTimeout(() => ws.send('Hello again!'), 3000)
}
ws.onerror = event => log('error: ' + event.message)
ws.onclose = () => log('close')
