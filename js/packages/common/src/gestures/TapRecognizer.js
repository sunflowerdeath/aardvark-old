import { PointerAction, PointerTool } from '../events/PointerEvent.js'

const isLeftMouseButtonPress = event =>
	event.tool === PointerTool.MOUSE &&
	event.action === PointerAction.BUTTON_PRESS // &&
// event.button === 0

const isLeftMouseButtonUp = event =>
	event.tool === PointerTool.MOUSE &&
	event.action === PointerAction.BUTTON_RELEASE // &&
// event.button === 0

const isTouchDown = event =>
	event.device === PointerTool.TOUCH &&
	event.action === PointerAction.POINTER_DOWN

const isTouchUp = event =>
	event.device === PointerTool.TOUCH &&
	event.action === PointerAction.POINTER_UP

class TapRecognizer {
	constructor({ document, onPressStart, onPressEnd, onTap }) {
		this.document = document
		this.onPressStart = onPressStart
		this.onPressEnd = onPressEnd
		this.onTap = onTap

		this.onPointerEvent = this.onPointerEvent.bind(this)
		this.press = this.press.bind(this)
		this.unpress = this.unpress.bind(this)
		this.onGestureResolve = this.onGestureResolve.bind(this)
	}

	handler(event, eventType) {
        if (this.isDestroyed) return
		if (this.isPressed && event.pointerId === this.pressedPointer) {
			if (isTouchUp(event)) {
				if (this.isAccepted && this.onTap) this.onTap(event)
				this.unpress(event)
			} else if (isLeftMouseButtonUp(event)) {
				if (this.onTap) this.onTap(event)
				this.unpress(event)
			}
			return
		}

		if (isLeftMouseButtonPress(event) || isTouchDown(event)) {
			this.press(event)
		}
	}

    destroy() {
        this.isDestroyed = true
        if (this.isPressed) this.unpress()
    }

	onPointerEvent(event) {
		if (isTouchUp(event) || isLeftMouseButtonUp(event)) this.unpress(event)
	}

	press(event) {
        log('PRESS')
		this.stopTrackingPointer = this.document().startTrackingPointer(
			event.pointerId,
			this.onPointerEvent
		)
		this.pressedPointer = event.pointerId
		if (event.tool === PointerTool.TOUCH) {
			this.gestureResolverEntry = gestureResolver.addEntry(
				event.pointerId,
				this.onGestureResolve
			)
		}
		this.isPressed = true
		if (this.onPressStart) this.onPressStart()
	}

	onGestureResolve(isAccepted) {
		this.isAccepted = isAccepted
		// TODO cancel
	}

	unpress(event) {
		this.stopTrackingPointer()
		this.isPressed = false
		this.isAccepted = false
		if (this.onPressEnd) this.onPressEnd()
	}
}

export default TapRecognizer
