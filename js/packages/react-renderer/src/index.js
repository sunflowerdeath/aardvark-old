import Reconciler from 'react-reconciler'
import { connectToDevTools } from 'react-devtools-core'
import WebApiWebSocket from '@advk/common/src/WebApiWebSocket'
import hostConfig from './hostConfig.js'

const Renderer = Reconciler(hostConfig)

const websocket = new WebApiWebSocket('ws://localhost:8097')
// ws.onopen = () => log('open')
// ws.onerror = event => log('error:' + event.message)
connectToDevTools({ websocket })

Renderer.injectIntoDevTools({
	findFiberByHostInstance: Renderer.findHostInstance,
	bundleType: 1,
	version: '0.0.1',
	rendererPackageName: 'react-aardvark'
})

const RendererAPI = {
	render(element, container, callback) {
		// create a root Container if it doesnt exist
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
