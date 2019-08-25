// let ws = new WebSocket()
// ws.addStartHandler(() => {
    // log('start')
    // ws.send('Hello, server')
// })
// ws.addCloseHandler(() => log('close'))
// ws.addErrorHandler(() => log('error'))
// ws.addMessageHandler(event => log('message: ' + event.data))

import WebWebSocket from '@advk/common/src/WebWebSocket'
const ws = new WebWebSocket('ws://192.168.1.1:1234')
ws.onstart = () => {
    log('start')
    ws.send('Hello')
}
ws.onmessage = event => log('message: ' + event.data)
ws.onerror = event => log('error')
ws.onclose = () => log('close')
