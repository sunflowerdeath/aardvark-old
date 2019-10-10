import {
	PointerTool,
	PointerAction,
	PointerEventType
} from '../events/PointerEvent.js'
import VelocityTracker from './VelocityTracker.js'

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
	event.tool === PointerTool.MOUSE &&
	event.action === PointerAction.BUTTON_PRESS

const isMouseButtonUp = (event, button = 0) =>
	event.tool === PointerTool.MOUSE &&
	event.action === PointerAction.BUTTON_RELEASE

const isTouchDown = event =>
	event.device === PointerTool.TOUCH &&
	event.action === PointerAction.POINTER_DOWN

const isTouchUp = event =>
	event.device === PointerTool.TOUCH &&
	event.action === PointerAction.POINTER_UP

class DragRecognizer {
	constructor(options) {
		this.velocityTracker = new VelocityTracker()
		this.document = options.document
		this.onDragStart = options.onDragStart
		this.onDragEnd = options.onDragEnd
		this.onDragMove = options.onDragMove
		this.treshold = 'treshold' in options ? treshold : DEFAULT_TRESHOLD
		this.axis = 'axis' in options ? options.axis : DragAxis.VERT
		this.tool = 'tool' in options ? options.tool : DragTool.ANY
		// this.startPoint - touch or activation
		// inside/outside
	}

	makeDragEvent(event) {
		const deltaLeft = event.left - this.startEvent.left
		const deltaTop = event.top - this.startEvent.top
		this.velocityTracker.addPoint(event.timestamp, deltaTop)
        const velocity = this.velocityTracker.getVelocity()
		return { ...event, deltaLeft, deltaTop, velocity }
	}

	handler(event, eventType) {
		if (this.isStarted) return

		if (
			((this.tool === DragTool.ANY || this.tool === DragTool.MOUSE) &&
				isMouseButtonPress(event)) ||
			((this.tool === DragTool.ANY || this.tool === DragTool.TOUCH) &&
				isTouchDown(event))
		) {
			this.start(event)
		}
	}

	start(event) {
		this.isStarted = true
		this.startEvent = event

		this.isActive = true
		this.stopTrackingPointer = this.document().startTrackingPointer(
			event.pointerId,
			this.onPointerEvent.bind(this)
		)
		if (this.onDragStart) this.onDragStart(this.makeDragEvent(event))
	}

	onPointerEvent(event) {
		if (isTouchUp(event) || isMouseButtonUp(event)) {
			this.end(event)
		} else {
			this.update(event)
		}
	}

	update(event) {
		if (this.isActive) {
			if (this.onDragMove) this.onDragMove(this.makeDragEvent(event))
		}
	}

	end(event) {
		this.stopTrackingPointer()
		this.isStarted = false
		this.isActive = false
		if (this.onDragEnd) this.onDragEnd(this.makeDragEvent(event))
	}
}

export default DragRecognizer
export { DragAxis, DragTool }
