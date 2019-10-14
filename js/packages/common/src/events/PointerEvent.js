export const PointerAction = {
	POINTER_DOWN: 0,
	POINTER_UP: 1,
	BUTTON_PRESS: 3,
	BUTTON_RELEASE: 4
}

export const PointerTool = {
	TOUCH: 0,
	MOUSE: 1,
	OTHER: 2
}

export const PointerEventType = {
	ENTER: 0,
	LEAVE: 1,
	CHANGE: 2
}

export const isMouseButtonPress = (event, button) =>
	event.tool === PointerTool.MOUSE &&
	event.action === PointerAction.BUTTON_PRESS &&
	(button === undefined || event.button === button)
