const isMouseButtonPress = (event, button) =>
	event.tool === PointerTool.mouse &&
	event.action === PointerAction.buttonPress &&
	(button === undefined || event.button === button)

export default isMouseButtonPress
