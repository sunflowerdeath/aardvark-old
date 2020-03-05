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
	event.tool === PointerTool.mouse &&
	event.action === PointerAction.buttonPress

const isMouseButtonUp = (event, button = 0) =>
	event.tool === PointerTool.mouse &&
	event.action === PointerAction.buttonRelease

const isTouchDown = event =>
	event.device === PointerTool.touch &&
	event.action === PointerAction.pointerDown

const isTouchUp = event =>
	event.device === PointerTool.touch &&
	event.action === PointerAction.pointerUp

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
        this.isDisabled = 'isDisabled' in options ? options.isDisabled : false
		// this.startPoint - touch or activation
		// inside/outside
	}

	makeDragEvent(event) {
		const deltaLeft = event.left - this.startEvent.left
		const deltaTop = event.top - this.startEvent.top
		this.velocityTracker.addPoint(event.timestamp, deltaTop)
		const velocity = this.velocityTracker.getVelocity()
		return { originalEvent: event, deltaLeft, deltaTop, velocity }
	}

	handler(event, eventType) {
        if (this.isDisabled) return
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

    destroy() {
        this.end()
    }

	start(event) {
		this.isStarted = true
		this.startEvent = event

		this.isActive = true
		this.trackingPointerConnection = this.document().startTrackingPointer(
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
        if (!this.isStarted) return
		this.trackingPointerConnection.disconnect()
		this.isStarted = false
		this.isActive = false
		if (this.onDragEnd) this.onDragEnd(this.makeDragEvent(event))
	}

    enable() {
        this.isDisabled = false
    }

    disable() {
        this.isDisabled = true
        this.end()
    }
}

export default DragRecognizer
export { DragAxis, DragTool }
