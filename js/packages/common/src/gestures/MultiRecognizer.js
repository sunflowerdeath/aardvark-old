class MultiRecognizer {
    constructor(recognizers) {
        this.recognizers = recognizers
    }

    handler(event, eventType) {
        for (const key in this.recognizers) {
            this.recognizers[key].handler(event, eventType)
        }
    }

    destroy() {
        for (const key in this.recognizers) {
            this.recognizers[key].destroy()
        }
    }

    enable() {
        for (const key in this.recognizers) {
            this.recognizers[key].enable()
        }

    }

    disable() {
        for (const key in this.recognizers) {
            this.recognizers[key].disable()
        }
    }
}

export default MultiRecognizer
