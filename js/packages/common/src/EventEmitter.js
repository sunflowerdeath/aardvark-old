class EventEmitter {
	constructor(eventTypes) {
		this.eventTypes = eventTypes
		this.properties = {}
		this.listeners = {}
		eventTypes.map(type => {
			this.listeners[type] = []
		})
	}

	isValidEventType(type) {
		return this.eventTypes.indexOf(type) !== -1
	}

	setEventProperty(eventType, value) {
		if (!this.isValidEventType(eventType)) return
        // TODO check value type
		this.properties[eventType] = value
	}

	getEventProperty(eventType, value) {
		if (!this.isValidEventType(eventType)) return
		this.properties[eventType] = value
	}

	addEventListener(eventType, value) {
		if (!this.isValidEventType(eventType)) return
		this.listeners[event].push(value)
	}

	removeEventListener(eventType, value) {
		if (!this.isValidEventType(eventType)) return
		const listeners = this.listeners[eventType]
		const index = listeners.indexOf(value)
		if (index !== -1) listeners.splice(index, 1)
	}

	dispatchEvent(eventType, ...args) {
		if (!this.isValidEventType(eventType)) return
		this.listeners[eventType].map(listener => listener(...args))
		if (this.properties[eventType]) this.properties[eventType](...args)
	}
}

export default EventEmitter
