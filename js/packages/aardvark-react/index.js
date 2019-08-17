const Reconciler = require('react-reconciler')

const elements = {
	align: Align,
    center: Center,
	background: Background,
	responder: Responder,
	sized: Sized,
	stack: Stack,
    text: Text
	// placeholder: Placeholder
}

const getRootHostContext = rootContainerInstance => {
	// log('getRootHostContext')
	return {}
}

const getChildHostContext = (
	parentHostContext,
	type,
	rootContainerInstance
) => {
	// log('getChildHostContext')
	return parentHostContext
}

const getPublicInstance = instance => {
	// log('getPublicInstance')
	return instance
}

const prepareForCommit = containerInfo => {
	// Noop
}

const resetAfterCommit = containerInfo => {
	// Noop
}

const createInstance = (
	type,
	props,
	rootContainerInstance,
	hostContext,
	internalInstanceHandle
) => {
	// log('createInstance ' + type)
	const elem = new elements[type]()
	for (const prop in props) {
		if (prop === 'children') continue
		if (type === 'responder' && prop === 'handler') {
			elem.setHandler(props[prop])
		} else {
			elem[prop] = props[prop]
		}
	}
	return elem
}

const appendInitialChild = (parentInstance, child) => {
	// log('appendInitialChild')
	parentInstance.appendChild(child)
}

const finalizeInitialChildren = (
	parentInstance,
	type,
	props,
	rootContainerInstance,
	hostContext
) => {
	// log('finalizeInitialChildren')
	return false
}

const prepareUpdate = (
	instance,
	type,
	oldProps,
	newProps,
	rootContainerInstance,
	hostContext
) => {
	// Computes the diff for an instance. Fiber can reuse this work even if it
	// pauses or abort rendering a part of the tree.
	// log('prepareUpdate')
	return true
}

const shouldSetTextContent = (type, props) => {
	// log('shouldSetTextContent')
	return false
}

const shouldDeprioritizeSubtree = (type, props) => {
	// log('shouldDeprioritizeSubtree')
	return false
}

const createTextInstance = (
	text,
	rootContainerInstance,
	hostContext,
	internalInstanceHandle
) => {
	// log('createTextInstance: ' + text)
}

const scheduleTimeout = setTimeout

const cancelTimeout = clearTimeout

const noTimeout = 0

const now = Date.now

const isPrimaryRenderer = true

const warnsIfNotActing = true

const supportsMutation = true

const appendChild = (parentInstance, child) => {
	// log('appendChild')
	parentInstance.appendChild(child)
}

const appendChildToContainer = (parentInstance, child) => {
	// log('appendChildToContainer')
	parentInstance.root = child
}

const commitTextUpdate = (textInstance, oldText, newText) => {
	// log('commitTextUpdate')
	textInstance.text = newText
}

const commitMount = (instance, type, newProps, internalInstanceHandle) => {
	// Noop
}

const commitUpdate = (
	instance,
	updatePayload,
	type,
	oldProps,
	newProps,
	internalInstanceHandle
) => {
	// log('commitUpdate')
	for (const key in oldProps) {
		if (!(key in newProps)) {
		} // delete prop
	}
	for (const key in newProps) {
		if (!(key in oldProps) || oldProps[key] !== newProps[key]) {
			if (key === 'children') continue
			if (type === 'responder' && key === 'handler') {
				// instance.setHandler(newProps[key])
			} else {
				instance[key] = newProps[key]
			}
		}
	}
}

const insertBefore = (parentInstance, child, beforeChild) => {
	// TODO Move existing child or add new child?
	// log('insertBeforeChild')
	parentInstance.insertBeforeChild(child, beforeChild)
}
const insertInContainerBefore = (parentInstance, child, beforeChild) => {
	// log('Container does not support insertBefore operation')
}

const removeChild = (parentInstance, child) => {
	// log('removeChild')
	parentInstance.removeChild(child)
}

const removeChildFromContainer = (parentInstance, child) => {
	// log('removeChildFromContainer')
	// TODO placeholder
	parentInstance.root = new Stack()
}

const resetTextContent = instance => {
	// Noop
}

const HostConfig = {
	getPublicInstance,
	getRootHostContext,
	getChildHostContext,
	prepareForCommit,
	resetAfterCommit,
	createInstance,
	appendInitialChild,
	finalizeInitialChildren,
	prepareUpdate,
	shouldSetTextContent,
	shouldDeprioritizeSubtree,
	createTextInstance,
	scheduleTimeout,
	cancelTimeout,
	noTimeout,
	now,
	isPrimaryRenderer,
	warnsIfNotActing,
	supportsMutation,
	appendChild,
	appendChildToContainer,
	commitTextUpdate,
	commitMount,
	commitUpdate,
	insertBefore,
	insertInContainerBefore,
	removeChild,
	removeChildFromContainer,
	resetTextContent
}

const Renderer = Reconciler(HostConfig)

const RendererAPI = {
	render(element, container, callback) {
		// log('render')
		// Create a root Container if it doesnt exist
		if (!container._rootContainer) {
			container._rootContainer = Renderer.createContainer(
				container,
				false
			)
		}

		// update the root Container
		return Renderer.updateContainer(
			element,
			container._rootContainer,
			null,
			callback
		)
	}
}

module.exports = RendererAPI
