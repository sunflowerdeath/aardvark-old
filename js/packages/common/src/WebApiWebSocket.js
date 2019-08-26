import url from 'url'

import EventEmitter from './EventEmitter.js'

const WebSocketState = {
	CONNECTING: 0,
	OPEN: 1,
	CLOSING: 2,
	CLOSED: 3
}

const WEBSOCKET_EVENTS = ['start', 'error', 'close', 'message']

class WebApiWebSocket {
	constructor(urlString) {
		let parsedUrl
		try {
			parsedUrl = new url.parse(urlString)
		} catch (e) {
			throw new Error(`Invalid URL: '${urlString}'.`)
		}

		if (parsedUrl.protocol !== 'ws:') {
			throw new Error("WebSocket URL must have 'ws' protocol.")
		}

		const host = parsedUrl.hostname
		const port = parsedUrl.port === null ? '80' : parsedUrl.port

		this.eventEmitter = new EventEmitter(WEBSOCKET_EVENTS)
		this.webSocket = new WebSocket(host, port)
        this.webSocket.addStartHandler(() => {
            this.eventEmitter.dispatchEvent('start')
        })
        this.webSocket.addMessageHandler(event =>
            this.eventEmitter.dispatchEvent('message', event)
        )
        this.webSocket.addErrorHandler(event =>
            this.eventEmitter.dispatchEvent('error', event)
        )
        this.webSocket.addCloseHandler(() =>
            this.eventEmitter.dispatchEvent('close')
        )
		Object.assign(this, WebSocketState)
	}

	send(value) {
		this.webSocket.send(value)
	}

	close() {
		this.nativeWebSocket.close()
	}

	get readyState() {
		return this.nativeWebSocket.state
	}

	get onopen() {
		this.eventEmitter.getEventProperty('start')
	}
	set onopen(value) {
        log('set')
		this.eventEmitter.setEventProperty('start', value)
	}

	get onmessage() {
		this.eventEmitter.getEventProperty('message')
	}
	set onmessage(value) {
		this.eventEmitter.setEventProperty('message', value)
	}

	get onclose() {
		this.eventEmitter.getEventProperty('close')
	}
	set onclose(value) {
		this.eventEmitter.setEventProperty('close', value)
	}

	get onerror() {
		this.eventEmitter.getEventProperty('error')
	}
	set onerror(value) {
		this.eventEmitter.setEventProperty('error', value)
	}

	addEventListener(event, value) {
		this.eventEmitter.addEventListener(event, value)
	}

	removeEventListener(event, value) {
		this.eventEmitter.removeEventListener(event, value)
	}
}

Object.assign(WebApiWebSocket, WebSocketState)

export default WebApiWebSocket
