const Reconciler = require('react-reconciler')

const appendInitialChild = (parentInstance, child) => {
	log('appendInitialChild')
	parentInstance.appenChild(child)
}
const createInstance = (
	type,
	props,
	rootContainerInstance,
	hostContext,
	internalInstanceHandle
) => {
	log('createInstance')
}

const createTextInstance = (
	text,
	rootContainerInstance,
	hostContext,
	internalInstanceHandle
) => {
	log('createTextInstance: ' + text)
}

const finalizeInitialChildren = (
	parentInstance,
	type,
	props,
	rootContainerInstance,
	hostContext
) => {
	log('finalizeInitialChildren')
}

const getRootHostContext = rootContainerInstance => {
	log('getRootHostContext')
	return {}
}

const getChildHostContext = (
	parentHostContext,
	type,
	rootContainerInstance
) => {
	log('getChildHostContext')
	return parentHostContext
}

const getPublicInstance = instance => {
	log('getPublicInstance')
	return instance
}

const prepareForCommit = containerInfo => {
	// Noop
}

const UPDATE_SIGNAL = {}

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
	log('prepareUpdate')
	return UPDATE_SIGNAL
}

const resetAfterCommit = containerInfo => {
	// Noop
}

const isPrimaryRenderer = true
const warnsIfNotActing = true

const scheduleTimeout = setTimeout
const cancelTimeout = clearTimeout
const noTimeout = -1

const shouldDeprioritizeSubtree = (type, props) => {
	log('shouldDeprioritizeSubtree')
	return false
}

const shouldSetTextContent = (type, props) => {
	log('shouldSetTextContent')
	return false
}

const supportsMutation = true

const appendChild = (parentInstance, child) => {
	log('appendChild')
	parentInstance.appendChild(child)
}

const appendChildToContainer = (parentInstance, child) => {
	log('appendChildToContainer')
	parentInstance.root = child
}

const commitTextUpdate = (textInstance, oldText, newText) => {
	log('commitTextUpdate')
	textInstance.text = newText
}

const commitMount = (instance, type, newProps, internalInstanceHandle) => {
	// Noop
}

const commitUpdate = (
	instance,
	updatePayloadTODO,
	type,
	oldProps,
	newProps,
	internalInstanceHandle
) => {
	log('commitUpdate')
}

const insertBefore = (parentInstance, child, beforeChild) => {
	// TODO Move existing child or add new child?
	parentInstance.insertBefore(beforeChild, child)
}

const insertInContainerBefore = (parentInstance, child, beforeChild) => {
	log('Container does not support insertBefore operation')
}

const removeChild = (parentInstance, child) => {
	log('removeChild')
	parentInstance.removeChild(child)
}

const removeChildFromContainer = (parentInstance, child) => {
	log('removeChildFromContainer')
	parentInstance.root = new Placeholder()
}

const resetTextContent = instance => {
	// Noop
}

const HostConfig = {
    now
}

const MyRenderer = Reconciler(HostConfig)

const RendererPublicAPI = {
	render(element, container, callback) {
		// Call MyRenderer.updateContainer() to schedule changes on the roots.
		// See ReactDOM, React Native, or React ART for practical examples.
	}
}

module.exports = RendererPublicAPI
