import Reconciler from 'react-reconciler'
import { createElement, updateElement } from './helpers.js'

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
	return createElement(type, props)
}

const appendInitialChild = (parentInstance, child) => {
	// log('appendInitialChild')
    log(parentInstance.name)
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
    log(parentInstance.name)
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
	updateElement(instance, type, oldProps, newProps)
}

const insertBefore = (parentInstance, child, beforeChild) => {
	// TODO Move existing child or add new child?
	// log('insertBeforeChild')
    log(parentInstance.name)
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
	// TODO undefined / placeholder
	parentInstance.root = new Placeholder()
}

const resetTextContent = instance => {
	// Noop
}

const hostConfig = {
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

export default Reconciler(hostConfig)
