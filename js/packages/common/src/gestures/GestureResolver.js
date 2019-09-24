import GestureArena from './GestureArena'

class GestureResolver {
    constructor(document) {
        this.document = document
        document.addHandler(this.onPointerEvent.bind(this))
        document.addHandler(this.afterPointerEvent.bind(this), true)
    }

    onPointerEvent(event) {
        if (event.action === PointerEventAction.POINTER_DOWN) {
            this.arenas[event.pointerId] = new GestureArena(this)
        } else if (event.action === PointerEventAction.POINTER_UP) {
            this.arenas[event.pointerId].resolve()
            delete this.arenas[event.pointerId]
        }
    }

    afterPointerEvent(event) {
        if (event.action === PointerEventAction.POINTER_DOWN) {
            this.arenas[event.pointerId].close()
        }
    }

    addEntry(pointerId, handler) {
        return this.arenas[pointerId].addEntry(handler)
    }
}

export default GestureResolver