import Reconciler from 'react-reconciler'

import WebApiWebSocket from '@advk/common/src/WebApiWebSocket'
import { connectToDevTools } from "react-devtools-core";

import { createElement, updateElement } from './elements.js'

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
    updateElement(instance, type, oldProps, newProps)
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
	// TODO undefined
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

const websocket = new WebApiWebSocket('ws://localhost:8097')
// ws.onopen = () => log('open')
// ws.onerror = event => log('error:' + event.message)
connectToDevTools({ websocket })

Renderer.injectIntoDevTools({
    findFiberByHostInstance: Renderer.findHostInstance,
    bundleType: 1,
    version: '0.0.1',
    rendererPackageName: 'react-aardvark',
})

const RendererAPI = {
	render(element, container, callback) {
		// Create a root Container if it doesnt exist
		if (!container._rootContainer) {
			container._rootContainer = Renderer.createContainer(
				container,
				false
			)
		}

		// update the root Container
		Renderer.updateContainer(
			element,
			container._rootContainer,
			null,
			callback
		)

	}
}


export default RendererAPI
