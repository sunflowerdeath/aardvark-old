import { PointerEventType, PointerTool } from '../events/PointerEvent.js'

class HoverRecognizer {
    constructor({ onHoverStart, onHoverEnd }) {
        this.onHoverStart = onHoverStart
        this.onHoverEnd = onHoverEnd
        this.isDisabled = false
    }

    handler(event, eventType) {
        if (this.isDisabled) return
        if (event.tool !== PointerTool.MOUSE) return
        if (eventType === PointerEventType.ENTER) {
            this.onHoverStart && this.onHoverStart()
        } else if (eventType === PointerEventType.LEAVE) {
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
