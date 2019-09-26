import {
	PointerEventTool,
	PointerEventType,
	PointerEventAction
} from '../pointerEvents.js'

const DEFAULT_TRESHOLD = 12

const DragAxis = {
	VERT: 0,
	HORIZ: 1,
	BOTH: 2
}

const DragTool = {
	MOUSE: 0,
	TOUCH: 1,
	ANY: 2
}

const isMouseButtonPress = (event, button = 0) =>
	event.tool === PointerEventTool.MOUSE &&
	event.action === PointerEventAction.BUTTON_PRESS

const isMouseButtonUp = (event, button = 0) =>
	event.tool === PointerEventTool.MOUSE &&
	event.action === PointerEventAction.BUTTON_RELEASE

const isTouchDown = event =>
	event.device === PointerEventTool.TOUCH &&
	event.action === PointerEventAction.POINTER_DOWN

const isTouchUp = event =>
	event.device === PointerEventTool.TOUCH &&
	event.action === PointerEventAction.POINTER_UP

class DragRecognizer {
	constructor(options) {
		this.onDragStart = options.onDragStart
		this.onDragEnd = options.onDragEnd
		this.onDragMove = options.onDragMove
		this.treshold = 'treshold' in options ? treshold : DEFAULT_TRESHOLD
		this.axis = 'axis' in options ? options.axis : DragAxis.VERT
		this.tool = 'tool' in options ? options.tool : DragTool.ANY
		// this.startPoint - touch or activation
		// inside/outside
	}

	handler(event, eventType) {
		if (this.isStarted) {
			if (event.pointerId === this.startEvent.pointerId) {
				if (isTouchUp(event) || isMouseButtonUp(event)) {
                    this.end(event)
                } else {
                    this.update(event)
                }
            }
		} else {
			if (
				((this.tool === DragTool.ANY || this.tool === DragTool.MOUSE) &&
					isMouseButtonPress(event)) ||
				((this.tool === DragTool.ANY || this.tool === DragTool.TOUCH) &&
					isTouchDown(event))
			) {
				this.start(event)
			}
		}
	}

	start(event) {
		this.isStarted = true
		this.startEvent = event

		this.isActive = true
		if (this.onDragStart) this.onDragStart()
	}

	update(event) {
		if (this.isActive) {
			if (this.onDragMove) {
				const dragEvent = {
					deltaLeft: event.left - this.startEvent.left,
					deltaTop: event.top - this.startEvent.top
				}
				this.onDragMove(dragEvent)
			}
		}
	}

	end(event) {
		this.isStarted = false
		this.isActive = false
		if (this.onDragEnd) this.onDragEnd()
	}
}

export default DragRecognizer
export { DragAxis, DragTool }
