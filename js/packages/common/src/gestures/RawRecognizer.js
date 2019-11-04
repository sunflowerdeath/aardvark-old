class RawRecognizer {
	constructor(options) {
        this.options = options
	}

	handler(event, eventType) {
        if (this.isDisabled) return
        this.options.handler(event, eventType)
	}

    destroy() {
        if (this.options.destroy) this.options.destroy()
    }

    enable() {
        this.isDisabled = false
    }

    disable() {
        this.isDisabled = true
    }
}

export default RawRecognizer
