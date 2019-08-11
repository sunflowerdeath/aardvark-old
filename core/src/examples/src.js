import {
    GestureResolver, PointerEventAction, PointerEventTool 
} from 'aardvark-std'

let app = new DesktopApp()
log('create app')
let window = app.createWindow(640, 480)
log('create window')
log('app.windows.size =', app.windows.size)
log('app.windows[0].width =', app.windows[0].width)
// TODO window.document ?
let doc = app.getDocument(window)
log('get doc')

const gestureResolver = new GestureResolver(doc)

const isLeftMouseButtonPress = event =>
    event.tool === PointerEventTool.MOUSE &&
    event.action === PointerEventAction.BUTTON_PRESS // &&
    // event.button === 0

const isLeftMouseButtonUp = event =>
    event.tool === PointerEventTool.MOUSE &&
    event.action === PointerEventAction.BUTTON_RELEASE // &&
    // event.button === 0

const isTouchDown = event =>
    event.device === PointerEventTool.TOUCH &&
    event.action === PointerEventAction.POINTER_DOWN

const isTouchUp = event =>
    event.device === PointerEventTool.TOUCH &&
    event.action === PointerEventAction.POINTER_UP

class Button {
    constructor(args) {
        Object.assign(this, args)
    }

    make() {
        let elem = new Responder()
        elem.appendChild(this.child)
        elem.setHandler(this.onResponderEvent.bind(this))
        return elem
    }

    onResponderEvent(event) {
        if (this.isPressed && event.pointerId === this.pressedPointer) {
            if (isTouchUp(event)) {
                if (this.isAccepted) this.onClick(event)
                this.unpress()
            } else if (isLeftMouseButtonUp(event)) {
                this.unpress()
                this.onClick(event)
            }
            return
        }

        if (isLeftMouseButtonPress(event) || isTouchDown(event)) {
            this.press(event)
        }
    }

    onPointerEvent(event) {
        if (isTouchUp(event) || isLeftMouseButtonUp(event)) this.unpress()
    }

    press(event) {
        this.onPress()
        this.isPressed = true
        this.pressedPointer = event.pointerId
        this.stopTrackingPointer = doc.startTrackingPointer(
            event.pointerId,
            this.onPointerEvent.bind(this)
        )
        if (event.device === PointerEventDevice.TOUCH) {
            this.gestureResolverEntry = gestureResolver.addEntry(
                event.pointerId,
                this.onGestureResolve.bind(this)
            )
        }
    }

    onGestureResolve(isAccepted) {
        this.isAccepted = isAccepted
        if (!isAccepted) this.unpress()
    }

    unpress() {
        this.onUnpress()
        this.isPressed = false
        this.isAccepted = false
        this.stopTrackingPointer()
    }
}

let stack = new Stack()

let text = new Text("Hello World")
log(text.text)
text.text = "Hello, Text"
stack.appendChild(text)

let bg = new Background()
let onClick = () => log('click')
let onPress = () => log('press')
let onUnpress = () => log('unpress')
let button = new Button({ child: bg, onClick, onPress, onUnpress })
let buttonElem = button.make()

let sized = new Sized()
sized.sizeConstraints = {
    minWidth: {type: 'abs', value: 200},
    maxWidth: {type: 'abs', value: 200},
    minHeight: {type: 'abs', value: 50},
    maxHeight: {type: 'abs', value: 50}
}
let align = new Align()
align.align = {
    left: {type: 'abs', value: 50},
    top: {type: 'abs', value: 100}
}

sized.appendChild(buttonElem)
align.appendChild(sized)
stack.appendChild(align)

doc.root = stack

/*
let removeHandler
let handler = event => {
    log('pointer event ' + JSON.stringify(event))
    if (event.action == 3) removeHandler()
}
removeHandler = doc.addHandler(handler)

let stopTracking
let trackHandler = event => {
    log('tracked pointer ' + JSON.stringify(event))
    if (event.action == 3) stopTracking()
}
stopTracking = doc.startTrackingPointer(0, trackHandler)
*/

app.run()
