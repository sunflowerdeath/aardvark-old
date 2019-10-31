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
}

export default MultiRecognizer
