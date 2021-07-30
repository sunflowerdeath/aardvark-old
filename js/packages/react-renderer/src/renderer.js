import Reconciler from 'react-reconciler'
import { createElement, updateElement } from './helpers.js'

const logRenderCalls = false

const getRootHostContext = (rootContainerInstance) => {
    if (logRenderCalls) log('getRootHostContext')
    return {}
}

const getChildHostContext = (
    parentHostContext,
    type,
    rootContainerInstance
) => {
    if (logRenderCalls) log('getChildHostContext')
    return parentHostContext
}

const getPublicInstance = (instance) => {
    if (logRenderCalls) log('getPublicInstance')
    return instance
}

const prepareForCommit = (containerInfo) => {
    // Noop
}

const resetAfterCommit = (containerInfo) => {
    // Noop
}

const createInstance = (
    type,
    props,
    rootContainerInstance,
    hostContext,
    internalInstanceHandle
) => {
    if (logRenderCalls) log('createInstance ' + type)
    return createElement(type, props)
}

const appendInitialChild = (parentInstance, child) => {
    if (logRenderCalls) log('appendInitialChild')
    if (parentInstance.name === 'Paragraph') {
        parentInstance.root.appendChild(child)
    } else {
        parentInstance.appendChild(child)
    }
}

const finalizeInitialChildren = (
    parentInstance,
    type,
    props,
    rootContainerInstance,
    hostContext
) => {
    if (logRenderCalls) log('finalizeInitialChildren')
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
    if (logRenderCalls) // log('shouldSetTextContent')
    return false
}

const shouldDeprioritizeSubtree = (type, props) => {
    if (logRenderCalls) log('shouldDeprioritizeSubtree')
    return false
}

const createTextInstance = (
    text,
    rootContainerInstance,
    hostContext,
    internalInstanceHandle
) => {
    if (logRenderCalls) log('createTextInstance: ' + text)
}

const scheduleTimeout = setTimeout

const cancelTimeout = clearTimeout

const noTimeout = 0

const now = Date.now

const isPrimaryRenderer = true

const warnsIfNotActing = true

const supportsMutation = true

const appendChild = (parentInstance, child) => {
    if (logRenderCalls) log('appendChild')
    if (parentInstance.name == 'Paragraph') {
        parentInstance.root.appendChild(child)
    } else {
        parentInstance.appendChild(child)
    }
}

const appendChildToContainer = (parentInstance, child) => {
    if (logRenderCalls) log('appendChildToContainer')
    parentInstance.root = child
}

const commitTextUpdate = (textInstance, oldText, newText) => {
    if (logRenderCalls) log('commitTextUpdate')
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
    if (logRenderCalls) log('commitUpdate')
    updateElement(instance, type, oldProps, newProps)
}

const insertBefore = (parentInstance, child, beforeChild) => {
    // TODO Move existing child or add new child?
    if (logRenderCalls) log('insertBeforeChild')
    log(parentInstance.name)
    parentInstance.insertBeforeChild(child, beforeChild)
}
const insertInContainerBefore = (parentInstance, child, beforeChild) => {
    if (logRenderCalls) log('Container does not support insertBefore operation')
}

const removeChild = (parentInstance, child) => {
    if (logRenderCalls) log('removeChild')
    parentInstance.removeChild(child)
}

const removeChildFromContainer = (parentInstance, child) => {
    if (logRenderCalls) log('removeChildFromContainer')
    // TODO undefined / placeholder
    parentInstance.root = new PlaceholderElement()
}

const resetTextContent = (instance) => {
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
    resetTextContent,
}

export default Reconciler(hostConfig)
