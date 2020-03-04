class HoverRecognizer {
    constructor({ onHoverStart, onHoverEnd }) {
        this.onHoverStart = onHoverStart
        this.onHoverEnd = onHoverEnd
        this.isDisabled = false
    }

    handler(event, eventType) {
        if (this.isDisabled) return
        if (event.tool !== PointerTool.mouse) return
        if (eventType === ResponderEventType.add) {
            this.onHoverStart && this.onHoverStart()
        } else if (eventType === ResponderEventType.remove) {
            this.onHoverEnd && this.onHoverEnd()
        }
    }

    destroy() {
        // noop
    }

    enable() {
        this.isDisabled = false
    }

    disable() {
        this.isDisabled = true
    }
}

export default HoverRecognizer
