import Layer from './Layer'
import Size from './Size'
import ComposeOptions from './ComposeOptions'
import Position from './Position'

class Compositor {
	screen: Layer

	constructor(canvas: HTMLCanvasElement) {
		this.screen = new Layer(canvas)
	}

	getScreenLayer() {
		return this.screen
	}

	createOffscreenLayer(size: Size) {
		const canvas = document.createElement('canvas')
		canvas.width = size.width
		canvas.height = size.height
		return new Layer(canvas)
	}

	paintLayer(layer: Layer, pos: Position) {
		this.screen.ctx.drawImage(layer.canvas, pos.left, pos.top)
	}
}

export default Compositor
