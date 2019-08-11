import React, { useState, useEffect, useRef } from 'react'
import ReactDOMServer from 'react-dom/server'
import ReactAardvark from 'aardvark-react'

import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from 'aardvark-std'

let app = new DesktopApp()
let window = app.createWindow(640, 480)
let document = app.getDocument(window)

const INITIAL_STATE = { isPressed: false, isAccepted: false }

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

const onResponderEvent = (event, ctx) => {
	const { state, setState, props, ref } = ctx

	if (state.isPressed && event.pointerId === ref.current.pressedPointer) {
		if (isTouchUp(event)) {
			if (ref.current.isAccepted && props.onClick) props.onClick(event)
			unpress(event, ctx)
		} else if (isLeftMouseButtonUp(event)) {
			if (props.onClick) props.onClick(event)
			unpress(event, ctx)
		}
		return
	}

	if (isLeftMouseButtonPress(event) || isTouchDown(event)) {
		press(event, ctx)
	}
}

const onPointerEvent = (event, ctx) => {
	if (isTouchUp(event) || isLeftMouseButtonUp(event)) {
		unpress(event, ctx)
	}
}

const press = (event, ctx) => {
	log('press')
	const { state, setState, props, ref } = ctx
	ref.current = {
		stopTrackingPointer: document.startTrackingPointer(
			event.pointerId,
			event => onPointerEvent(event, ctx)
		),
		pressedPointer: event.pointerId
	}
	if (event.device === PointerEventTool.TOUCH) {
		ref.current.gestureResolverEntry = gestureResolver.addEntry(
			event.pointerId,
			isAccepted => onGestureResolve(isAccepted, ctx)
		)
	}
	setState({ isPressed: true })
	props.onChangeTapState && props.onChangeState({ isPressed: true })
}

const unpress = (event, ctx) => {
	log('unpress')
	const { state, setState, props, ref } = ctx
	ref.current.stopTrackingPointer()
	ref.current = undefined
    setState(INITIAL_STATE)
	props.onChangeTapState &&
		props.onChangeState({ isPressed: false, isHovered: false })
}

const onGestureResolve = (isAccepted, ctx) => {
	ctx.ref.current.isAccepted = isAccepted
}

const Button = props => {
	const { children, onClick, onChangeState } = props
	const [state, setState] = useState(INITIAL_STATE)
	const ref = useRef({})
	return (
		<responder
			handler={event => {
				onResponderEvent(event, { state, setState, props, ref })
			}}
		>
			{typeof children === 'function'
				? children({ isPressed: state.isPressed })
				: children}
		</responder>
	)
}

Button.propTypes = {
	/*
    children: PropTypes.oneOfType([ PropTypes.qwe, PropTypes.func]),
    onTap: PropTypes.func,
    onChangeState: PropTypes.func
    */
}

let App = () => {
	return (
		<align
			align={{
				left: { type: 'abs', value: 50 },
				top: { type: 'abs', value: 100 }
			}}
		>
			<Button onClick={() => log('click')}>
				<sized
					sizeConstraints={{
						minWidth: { type: 'abs', value: 200 },
						maxWidth: { type: 'abs', value: 200 },
						minHeight: { type: 'abs', value: 50 },
						maxHeight: { type: 'abs', value: 50 }
					}}
				>
					<background />
				</sized>
			</Button>
		</align>
	)
}

ReactAardvark.render(<App />, document)

app.run()
