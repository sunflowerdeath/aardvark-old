let ws = new WebSocket()
ws.addStartHandler(() => {
	log('start')
	ws.send('Hello, server')
})
ws.addCloseHandler(() => log('close'))
ws.addErrorHandler(() => log('error'))
ws.addMessageHandler(event => log('message: ' + event.data))
