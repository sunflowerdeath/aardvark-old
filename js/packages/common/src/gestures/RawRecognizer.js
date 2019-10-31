class RawRecognizer {
	constructor(options) {
        this.options = options
	}

	handler(event, eventType) {
        this.options.handler(event, eventType)
	}

    destroy() {
        if (this.options.destroy) this.options.destroy()
    }
}

export default RawRecognizer
