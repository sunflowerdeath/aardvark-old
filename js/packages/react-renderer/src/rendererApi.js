// import { connectToDevTools } from 'react-devtools-core'
// import WebApiWebSocket from '@advk/common/src/WebApiWebSocket'
import Renderer from './renderer'
import { registerNativeComponent } from './helpers'

const rootContainers = new Map()

const RendererAPI = {
    render(element, container, callback) {
        if (!rootContainers.has(container)) {
            rootContainers.set(
                container,
                Renderer.createContainer(container, false)
            )
        }

        Renderer.updateContainer(
            element,
            rootContainers.get(container),
            null,
            callback
        )
    },

    /*
    TODO
    connectToDevTools(url = 'ws://localhost:8097') {
        const websocket = new WebApiWebSocket(url)
        // ws.onopen = () => log('open')
        // ws.onerror = event => log('error:' + event.message)
        connectToDevTools({ websocket })
        Renderer.injectIntoDevTools({
	        findFiberByHostInstance: Renderer.findHostInstance,
	        bundleType: 1,
	        version: '0.0.1',
	        rendererPackageName: 'react-aardvark'
        })
    },
    */

    registerNativeComponent,
}

export default RendererAPI
