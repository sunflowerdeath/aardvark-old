import React, { useState, useEffect, useRef } from 'react'
import ReactAardvark from '@advk/react-renderer'
import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from '@advk/common'

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

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

let i = 0

const onResponderEvent = (event, ctx) => {
	const { state, setState, props, ref } = ctx

	if (state.isPressed && event.pointerId === ref.current.pressedPointer) {
		if (isTouchUp(event)) {
			if (ref.current.isAccepted && props.onClick) props.onClick(event)
			unpress(event, ctx)
		} else if (isLeftMouseButtonUp(event)) {
			if (props.onClick) props.onClick(event)
			// unpress(event, ctx)
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
	if (event.tool === PointerEventTool.TOUCH) {
		ref.current.gestureResolverEntry = gestureResolver.addEntry(
			event.pointerId,
			isAccepted => onGestureResolve(isAccepted, ctx)
		)
	}
	setState({ isPressed: true })
	props.onChangeState && props.onChangeState({ isPressed: true })
}

const unpress = (event, ctx) => {
	log('unpress')
	const { state, setState, props, ref } = ctx
	ref.current.stopTrackingPointer()
	ref.current = undefined
	setState(INITIAL_STATE)
	props.onChangeState && props.onChangeState({ isPressed: false })
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

const COLOR_RED = { alpha: 255, red: 0, green: 255, blue: 0 }
const COLOR_GREEN = { alpha: 255, red: 255, green: 0, blue: 0 }

const App = () => {
	log('render app')
	return (
		<stack>
			<align
				align={{
					left: { type: 'abs', value: 50 },
					top: { type: 'abs', value: 100 }
				}}
			>
				<Button onClick={() => log('click')}>
					{state => (
						<sized
							sizeConstraints={{
								minWidth: { type: 'abs', value: 200 },
								maxWidth: { type: 'abs', value: 200 },
								minHeight: { type: 'abs', value: 50 },
								maxHeight: { type: 'abs', value: 50 }
							}}
						>
							<stack>
								<background
									color={
										state.isPressed
											? COLOR_RED
											: COLOR_GREEN
									}
								/>
								<center>
									<text text="Test" />
								</center>
							</stack>
						</sized>
					)}
				</Button>
			</align>
			<align
				align={{
					left: { type: 'abs', value: 50 },
					top: { type: 'abs', value: 200 }
				}}
			>
				<Button onClick={() => {
                    log('exception')
                    const bg = new Background()
                    bg.color = { alpha: 255, red: 'red', green: 255, blue: 255 } 
                }}>
					{state => (
						<sized
							sizeConstraints={{
								minWidth: { type: 'abs', value: 200 },
								maxWidth: { type: 'abs', value: 200 },
								minHeight: { type: 'abs', value: 50 },
								maxHeight: { type: 'abs', value: 50 }
							}}
						>
						    <stack>
                                <background
								    color={
									    state.isPressed ? COLOR_RED : COLOR_GREEN
								    }
							    />
								<center>
									<text text="GC" />
								</center>
							</stack>
						</sized>
					)}
				</Button>
			</align>
			<align
				align={{
					left: { type: 'abs', value: 50 },
					top: { type: 'abs', value: 300 }
				}}
			>
			    <intrinsicHeight>
			        <stack>
			            <background color={COLOR_RED} />
			            <text text="INTRINSIC HEIGHT" />
			        </stack>
			    </intrinsicHeight>   
			</align>
		</stack>
	)
}

ReactAardvark.render(<App />, document)
// app.run()
// global.app = app
