import { PointerEventType, PointerTool } from '../events/PointerEvent.js'

class HoverRecognizer {
	constructor({ onHoverStart, onHoverEnd }) {
		this.onHoverStart = onHoverStart
		this.onHoverEnd = onHoverEnd
	}

	handler(event, eventType) {
		if (event.tool !== PointerTool.MOUSE) return
		if (eventType === PointerEventType.ENTER) {
			this.onHoverStart && this.onHoverStart()
		} else if (eventType === PointerEventType.LEAVE) {
			this.onHoverEnd && this.onHoverEnd()
		}
	}
}

export default HoverRecognizer
