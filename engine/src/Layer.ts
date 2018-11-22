import Size from './Size'
import ComposeOptions from './ComposeOptions'
import Element from './Element'

class Layer {
	canvas: HTMLCanvasElement
	ctx: CanvasRenderingContext2D
	size: Size
	composeOptions: ComposeOptions

	constructor(canvas: HTMLCanvasElement) {
		this.canvas = canvas
		this.ctx = canvas.getContext('2d')
		this.size = new Size({ width: canvas.width, height: canvas.height })
		this.composeOptions = ComposeOptions.defaultValue
	}

	clear() {
		this.ctx.clearRect(0, 0, this.size.width, this.size.height)
	}

	reset() {
		this.clear()
		this.composeOptions = ComposeOptions.defaultValue
	}
}

export default Layer
