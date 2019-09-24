import React, { useState, useEffect, useRef } from 'react'
import ReactAardvark from '@advk/react-renderer'
import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from '@advk/common'

const Value = {
	abs(value) {
		return { type: 'abs', value }
	},

	rel(value) {
		return { type: 'rel', value }
	},

	none: { type: 'none', value: 0 }
}

const Padding = {
	all(value) {
		return { left: value, top: value, right: value, bottom: value }
	},

	only(side, value) {
		return {
			left: side === 'left' ? value : 0,
			top: side === 'top' ? value : 0,
			right: side === 'right' ? value : 0,
			bottom: side === 'bottom' ? value : 0
		}
	},

	symmetrical(horiz, vert) {
		return { left: horiz, top: vert, right: horiz, bottom: vert }
	},

	horiz(value) {
		return { left: value, top: 0, right: value, bottom: 0 }
	},

	vert(value) {
		return { left: 0, top: value, right: 0, bottom: value }
	}
}

const FlexDirection = {
	row: 0,
	column: 1
}

const FlexAlign = {
	start: 0,
	center: 1,
	end: 2,
	stretch: 3
}

const FlexJustify = {
	start: 0,
	center: 1,
	end: 2,
	spaceAround: 3,
	spaceBetween: 4,
	spaceEvenly: 5
}

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

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

const PointerEventType = {
	enter: 0,
	leave: 1,
	chnage: 2
}

class HoverRecognizer {
	constructor({ onHoverStart, onHoverEnd }) {
		this.onHoverStart = onHoverStart
		this.onHoverEnd = onHoverEnd
	}

	handler(event, eventType) {
		if (event.tool !== PointerEventTool.MOUSE) return
		if (eventType === PointerEventType.enter) {
			this.onHoverStart && this.onHoverStart()
		} else if (eventType === PointerEventType.leave) {
			this.onHoverEnd && this.onHoverEnd()
		}
	}
}

class TapRecognizer {
	constructor({ onPressStart, onPressEnd, onTap }) {
		this.onPressStart = onPressStart
		this.onPressEnd = onPressEnd
		this.onTap = onTap

		this.onPointerEvent = this.onPointerEvent.bind(this)
		this.press = this.press.bind(this)
		this.unpress = this.unpress.bind(this)
		this.onGestureResolve = this.onGestureResolve.bind(this)
	}

	handler(event, eventType) {
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

	onPointerEvent(event) {
		if (isTouchUp(event) || isLeftMouseButtonUp(event)) this.unpress(event)
	}

	press(event) {
		this.stopTrackingPointer = document.startTrackingPointer(
			event.pointerId,
			this.onPointerEvent
		)
		this.pressedPointer = event.pointerId
		if (event.tool === PointerEventTool.TOUCH) {
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

class MultiRecognizer {
	constructor(recognizers) {
		this.recognizers = recognizers
	}

	handler(event, eventType) {
		for (const key in this.recognizers) {
			this.recognizers[key].handler(event, eventType)
		}
	}
}

const usePropsRef = (props, keys) => {
	const ref = useRef({})
	for (const key of keys) {
		useEffect(() => {
			ref.current[key] = props[key]
		}, [props[key]])
	}
	return ref.current
}

const Button = props => {
	const { children, onChangeTapState, onTap } = props

	const [state, setState] = useState({ isHovered: false, isPressed: false })
	const callbacks = usePropsRef(props, ['onChangeTapState', 'onTap'])

	const setTapState = tapState =>
		setState(prevState => {
			const nextState = { ...prevState, ...tapState }
			if (callbacks.onChangeTapState) {
				callbacks.onChangeTapState(nextState)
			}
			return nextState
		})

	const [recognizer] = useState(
		() =>
			new MultiRecognizer({
				hover: new HoverRecognizer({
					onHoverStart: () => setTapState({ isHovered: true }),
					onHoverEnd: () => setTapState({ isHovered: false })
				}),
				tap: new TapRecognizer({
					onPressStart: () => setTapState({ isPressed: true }),
					onPressEnd: () => setTapState({ isPressed: false }),
					onTap: () => {
						if (callbacks.onTap) onTap()
					}
				})
			})
	)

	return (
		<responder handler={recognizer.handler.bind(recognizer)}>
			{children(state)}
		</responder>
	)
}

const BTN_INITIAL_COLOR = { red: 238, green: 238, blue: 238, alpha: 255 }
const BTN_HOVERED_COLOR = { red: 165, green: 214, blue: 167, alpha: 255 }
const BTN_PRESSED_COLOR = { red: 255, green: 171, blue: 145, alpha: 255 }

const ExampleButton = ({ children, onTap }) => (
	<Button onTap={onTap}>
		{({ isHovered, isPressed }) => {
			return (
				<sized sizeConstraints={{ height: Value.abs(40) }}>
					<stack>
						<background
							color={
								isPressed
									? BTN_PRESSED_COLOR
									: isHovered
									? BTN_HOVERED_COLOR
									: BTN_INITIAL_COLOR
							}
						/>
						<center>
							<padding padding={Padding.horiz(16)}>
								<intrinsicWidth>{children}</intrinsicWidth>
							</padding>
						</center>
					</stack>
				</sized>
			)
		}}
	</Button>
)

const INITIAL_COLOR = { red: 179, green: 229, blue: 252, alpha: 255 }
const HOVERED_COLOR = { red: 79, green: 195, blue: 247, alpha: 255 }

const Panel = ({ children }) => {
	const [isHovered, setIsHovered] = useState(false)
	const [hoverRecognizer] = useState(
		() =>
			new HoverRecognizer({
				onHoverStart: () => setIsHovered(true),
				onHoverEnd: () => setIsHovered(false)
			})
	)

	return (
		<responder handler={hoverRecognizer.handler.bind(hoverRecognizer)}>
			<sized sizeConstraints={{ width: Value.abs(200) }}>
				<stack>
					<background
						color={isHovered ? HOVERED_COLOR : INITIAL_COLOR}
					/>
					<padding padding={Padding.all(16)}>
						<flex direction={FlexDirection.column}>{children}</flex>
					</padding>
				</stack>
			</sized>
		</responder>
	)
}

const App = () => {
	return (
		<stack>
			<background
				color={{ red: 255, green: 255, blue: 255, alpha: 255 }}
			/>
			<padding padding={Padding.all(16)}>
				<Panel>
					<ExampleButton onTap={() => log('Tap first button')}>
						<text text="Button 1" />
					</ExampleButton>
					<ExampleButton onTap={() => log('Tap second button')}>
						<text text="Button 2" />
					</ExampleButton>
				</Panel>
			</padding>
		</stack>
	)
}

ReactAardvark.render(<App />, document)
