// import { connectToDevTools } from 'react-devtools-core'
import WebApiWebSocket from '@advk/common/src/WebApiWebSocket'
import Renderer from './renderer.js'
import { registerNativeComponent } from './helpers.js'

const RendererAPI = {
	render(element, container, callback) {
		// create a root Container if it doesnt exist
		if (!container.rootContainer) {
			container.rootContainer = Renderer.createContainer(
				container,
				false
			)
		}

		// update the root Container
		Renderer.updateContainer(
			element,
			container.rootContainer,
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
    
    registerNativeComponent
}

export default RendererAPI
