import Element from '../Element'
import Size from '../Size'
import BoxConstraints from '../BoxConstraints'

interface BackgroundProps {
	color: string
}

class Background extends Element<BackgroundProps> {
	layout(constraints: BoxConstraints) {
		return new Size({
			width: constraints.maxWidth,
			height: constraints.maxHeight
		})
	}

	paint() {
		const layer = this.document.getLayer()
		const ctx = layer.canvas.getContext('2d')
		ctx.save()
		ctx.translate(this.absPosition.left, this.absPosition.top)
		ctx.fillStyle = this.props.color
		ctx.fillRect(0, 0, this.size.width, this.size.height)
		ctx.restore()
	}
}

export default Background
