
const elements = {
  center: Center,
  padding: Padding
}

/*
API

createElement(type: string, props: Object): Element

Element
  // updates element
  .setProps(changedProps: Object) 

  // how react knows that children is special props
  // so children should be not *replaced*, but *reconciled*, i.e.
  // changed children should be updated recursively

  .insertChild(child: Element, beforeChild?: Element)

  .removeChild(child: Element)
*/

const hostConfig = {
  getRootHostContext(rootContainerInstance) {
    console.log('getRootHostContext')
  },

  getChildHostContext(parentHostContext, type, rootContainerInstance) {
    console.log('getChildHostContext')
  },

  getPublicInstance(instance) {
    console.log('getPublicInstance');
  },

  prepareForCommit(containerInfo) {
    console.log('prepareForCommit');
  },

  resetAfterCommit(containerInfo) {
    console.log('resetAfterCommit')
  },

  createInstance(
    type,
    props,
    rootContainerInstance,
    hostContext,
    internalInstanceHandle
  ) {
    return createElement(type, props);
  },

  appendInitialChild(parentInstance, child) {
    // Initial childred are added in `createInstance`
  },

  finalizeInitialChildren(
    element,
    type,
    props,
    rootContainerInstance,
    hostContext
  ) {
    // Element is created completely in `createInstance`
  },

  // Calculates data needed to perform update of the component's props
  prepareUpdate(
    element,
    type,
    oldProps,
    newProps,
    rootContainerInstance,
    hostContext
  ) {
    const changedProps = {}
    for (let key in oldProps) {
      if (!(key in newProps)) {
        // Prop is removed
        changedProps[key] = undefined
      } else if (newProps[key] !== oldProps[key]) {
        // Prop is changed
        changedProps[key] = newProps[key]
      }
    }
    for (let key in newProps) {
      // Props is added
      if (!(key in oldProps)) changedProps[key] = newProps[key]
    }
    return changedProps
  },

  shouldSetTextContent(type, props) {
    console.log('shouldSetTextContent');
    return false
  },

  shouldDeprioritizeSubtree(type, props) {
    console.log('shouldDeprioritizeSubtree');
  },

  createTextInstance(
    text,
    rootContainerInstance,
    hostContext,
    internalInstanceHandle
  ) {
    console.log('createTextInstance');
  },

  now: null,

  isPrimaryRenderer: true,
  scheduleDeferredCallback: "",
  cancelDeferredCallback: "",

  // -------------------
  //     Mutation
  // -------------------

  supportsMutation: true,

  commitMount(domElement, type, newProps, internalInstanceHandle) {
    console.log('commitMount');
  },

  // Performs update of the component's props
  commitUpdate(
    element,
    updatePayload,
    type,
    oldProps,
    newProps,
    internalInstanceHandle
  ) {
    element.setProps(updatePayload);
  },

  resetTextContent(domElement) {
    console.log('resetTextContent');
  },

  commitTextUpdate(textInstance, oldText, newText) {
    console.log('commitTextUpdate', oldText, newText);
  },

  appendChildToContainer(container, child) {
    container.setRoot(child)
  },

  appendChild(parentInstance, child) {
    parent.insertChild(child)
  },

  insertBefore(parentInstance, child, beforeChild) {
    console.log('insertBefore');
    parentInstance.insertChild(child, beforeChild)
  },

  insertInContainerBefore(container, child, beforeChild) {
    console.log('insertInContainerBefore');
  },

  removeChild(parentInstance, child) {
    container.removeChild(child)
  },

  removeChildFromContainer(container, child) {
    console.log('removeChildFromContainer');
  }
};

const DOMRenderer = ReactReconciler(hostConfig);

let internalContainerStructure;
export default {
  render(elements, containerNode, callback) {

    // We must do this only once
    if (!internalContainerStructure) {
      internalContainerStructure = DOMRenderer.createContainer(
        containerNode,
        false,
        false
      );
    }

    DOMRenderer.updateContainer(elements, internalContainerStructure, null, callback);
  }
}
