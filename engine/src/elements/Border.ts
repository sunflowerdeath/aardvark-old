import Element from '../Element'
import Size from '../Size'
import BoxConstraints from '../BoxConstraints'
import Position from '../Position'
import Matrix from '../Matrix'

interface IRadius {
	width: number // TODO relative
	height: number
}

class Radius {
	width: number
	height: number

	constructor({ width, height }: IRadius) {
		this.width = width
		this.height = height
	}

	isZero(): boolean {
		return this.width === 0 && this.height === 0
	}

	inner(d: number): Radius {
		if (this.width <= d || this.height <= d) return Radius.zero
		return new Radius({ width: this.width - d, height: this.height - d })
	}

	static circular(radius: number): Radius {
		return new Radius({ width: radius, height: radius })
	}

	static zero = new Radius({ width: 0, height: 0 })
}

interface IBoxRadius {
	bottomLeft: Radius
	bottomRight: Radius
	topLeft: Radius
	topRight: Radius
}

class BoxRadius {
	bottomLeft: Radius
	bottomRight: Radius
	topLeft: Radius
	topRight: Radius

	constructor(radiuses: Partial<IBoxRadius>) {
		;(this.bottomLeft = radiuses.bottomLeft || Radius.zero),
			(this.bottomRight = radiuses.bottomRight || Radius.zero),
			(this.topLeft = radiuses.topLeft || Radius.zero),
			(this.topRight = radiuses.topRight || Radius.zero)
	}

	isSquare(): boolean {
		return [
			this.bottomLeft,
			this.bottomRight,
			this.topLeft,
			this.topRight
		].every(r => r.isZero())
	}

	static all(radius: Radius): BoxRadius {
		return new BoxRadius({
			bottomLeft: radius,
			bottomRight: radius,
			topLeft: radius,
			topRight: radius
		})
	}
}

interface IBorderSide {
	width: number
	color: string
}

class BorderSide {
	width: number
	color: string

	constructor(data: IBorderSide) {
		Object.assign(this, data)
	}

	static none = new BorderSide({ width: 0, color: null })

	static isEqual(a: BorderSide, b: BorderSide): boolean {
		return a.width === b.width && a.color === b.color
	}
}

interface IBoxBorder {
	top: BorderSide
	bottom: BorderSide
	left: BorderSide
	right: BorderSide
}

class BoxBorder {
	constructor({ top, bottom, left, right }: Partial<IBoxBorder>) {
		this.top = top || BorderSide.none
		this.bottom = bottom || BorderSide.none
		this.left = left || BorderSide.none
		this.right = right || BorderSide.none
	}

	top: BorderSide
	bottom: BorderSide
	left: BorderSide
	right: BorderSide

	static all({ width, color }: IBorderSide) {
		const side = new BorderSide({ width, color })
		return new BoxBorder({
			top: side,
			bottom: side,
			left: side,
			right: side
		})
	}
}

class PropsError extends Error {
	constructor(message) {
		super(message)
		this.message = message
	}
}

const degToRad = (deg: number): number => (Math.PI / 180) * deg

interface BorderProps {
	border: BoxBorder
	radius: BoxRadius
	child: Element<any>
}

const sides = ['top', 'right', 'bottom', 'left']
const corners = ['topLeft', 'topRight', 'bottomRight', 'bottomLeft']

const getIndex = (arr, i) => {
	if (i < 0) return arr[arr.length - 1]
	if (i >= arr.length) return arr[i % arr.length]
	return arr[i]
}

const checkBorders = ({ border, radius }: Partial<BorderProps>) => {
	for (let i = 0; i < 4; i++) {
		const corner = radius[getIndex(corners, i)]
		const leftSide = border[getIndex(sides, i)]
		const rightSide = border[getIndex(sides, i - 1)]
		if (!corner.isZero() && !BorderSide.isEqual(leftSide, rightSide)) {
			throw new PropsError(
				'Two adjacent borders can have rounded corner oonly if they ' +
					'have same thickness and color'
			)
		}
	}
}

class Border extends Element<BorderProps> {
	setProps(props: Partial<BorderProps>) {
		super.setProps(props)
		checkBorders(props)
	}

	layout(constraints: BoxConstraints) {
		const { border, child } = this.props
		const widths: { [name in keyof IBoxBorder]?: number } = {}
		for (let side of ['left', 'right', 'top', 'bottom']) {
			widths[side] = border[side] ? border[side].width : 0
		}
		const childConstraints = new BoxConstraints({
			minWidth: 0,
			maxWidth: constraints.maxWidth - widths.left - widths.right,
			minHeight: 0,
			maxHeight: constraints.maxHeight - widths.top - widths.bottom
		})
		const size = this.document.layoutElement(child, childConstraints)
		child.size = size
		child.relPosition = new Position({
			top: widths.top,
			left: widths.left
		})
		return new Size({
			width: size.width + widths.left + widths.right,
			height: size.height + widths.top + widths.bottom
		})
	}

	clipPath: Path2D
	transform: Matrix
	clipTransform: Matrix
	ctx: CanvasRenderingContext2D

	paint() {
		const { child, border, radius } = this.props

		const shouldClip = !radius.isSquare()
		if (shouldClip) {
			this.clipPath = new Path2D()
			this.clipTransform = new Matrix().translate(
				this.absPosition.left,
				this.absPosition.top
			)
			this.iterateSides(this.clipSide.bind(this))
		}

		this.ctx = this.document.getLayer().ctx
		this.ctx.save()
		this.document.paintElement(this.props.child, {
			clip: shouldClip && this.clipPath
		})

		this.transform = new Matrix()
		this.ctx.translate(this.absPosition.left, this.absPosition.top)
		this.iterateSides(this.paintSide.bind(this))
		this.ctx.restore()
	}

	iterateSides(callback) {
		const { border, radius } = this.props
		for (let i = 0; i < 4; i++) {
			callback({
				side: border[getIndex(sides, i)],
				prevSide: border[getIndex(sides, i - 1)],
				nextSide: border[getIndex(sides, i + 1)],
				leftRadius: radius[getIndex(corners, i)],
				rightRadius: radius[getIndex(corners, i + 1)],
				rotation: i * 90
			})
		}
	}

	clipSide({ prevSide, side, nextSide, leftRadius, rightRadius, rotation }) {
		const width = rotation % 180 === 0 ? this.size.width : this.size.height
		const begin = Math.max(prevSide.width, leftRadius.width)
		const end = width - Math.max(nextSide.width, rightRadius.width)
		this.clipPath.lineTo(...this.clipTransform.calc(begin, side.width))
		this.clipPath.lineTo(...this.clipTransform.calc(end, side.width))
		const innerRadius = rightRadius.inner(side.width)
		if (!innerRadius.isZero()) {
			// @ts-ignore error about number of arguments
			this.clipPath.ellipse(
				...this.clipTransform.calc(
					width - rightRadius.width,
					rightRadius.height
				),
				// making radius 1px bigger is needed to avoid artifacts 
				// between border and child
				innerRadius.width + 1,
				innerRadius.height + 1,
				0,
				degToRad(rotation - 90), // start angle
				degToRad(rotation) // end angle
			)
		}
		this.clipTransform.translate(width, 0).rotate(degToRad(90), 0, 0)
	}

	paintArc({ radius, side, width, rotation }) {
		this.ctx.beginPath()
		// todo radius width/height will change
		const lineWidth = Math.min(side.width, radius.width) // height???
		this.ctx.lineWidth = lineWidth
		// @ts-ignore error about number of arguments
		this.ctx.ellipse(
			...this.transform.calc(width - radius.width, radius.height), // left, top
			radius.width - lineWidth / 2, // radius x
			radius.height - lineWidth / 2, // radius y
			0, // ellipse rotation
			degToRad(rotation - 90), // start angle
			degToRad(rotation) // end angle
		)
		this.ctx.stroke()
	}

	paintTriangle({ radius, side, prevSide, nextSide, width, edge }) {
		if (edge === 'left' && prevSide.width === 0) return
		if (edge === 'right' && nextSide.width === 0) return
		this.ctx.beginPath()
		if (edge === 'left') {
			this.ctx.moveTo(...this.transform.calc(0, 0))
			this.ctx.lineTo(...this.transform.calc(prevSide.width, 0))
			this.ctx.lineTo(...this.transform.calc(prevSide.width, side.width))
		} else {
			this.ctx.moveTo(...this.transform.calc(width, 0))
			this.ctx.lineTo(...this.transform.calc(width - nextSide.width, 0))
			this.ctx.lineTo(
				...this.transform.calc(width - nextSide.width, side.width)
			)
		}
		this.ctx.fill()
	}

	paintSide({ prevSide, side, nextSide, leftRadius, rightRadius, rotation }) {
		const width = rotation % 180 === 0 ? this.size.width : this.size.height
		if (side.width === 0) {
			this.transform.translate(width, 0).rotate(degToRad(90), 0, 0)
			return
		}

		this.ctx.strokeStyle = side.color
		this.ctx.fillStyle = side.color
		if (leftRadius.isZero() && prevSide.color !== side.color) {
			this.paintTriangle({
				prevSide,
				side,
				nextSide,
				width,
				edge: 'left',
				radius: leftRadius
			})
		}
		this.ctx.lineWidth = side.width
		this.ctx.beginPath()
		this.ctx.moveTo(
			...this.transform.calc(
				leftRadius.isZero()
					? prevSide.color === side.color
						? 0
						: prevSide.width
					: leftRadius.width,
				side.width / 2
			)
		)
		this.ctx.lineTo(
			...this.transform.calc(
				width -
					(rightRadius.isZero() ? nextSide.width : rightRadius.width),
				side.width / 2
			)
		)
		this.ctx.stroke()
		if (rightRadius.isZero()) {
			if (prevSide.color !== side.color) {
				this.paintTriangle({
					prevSide,
					side,
					nextSide,
					width,
					edge: 'right',
					radius: rightRadius
				})
			}
		} else {
			this.paintArc({ side, width, radius: rightRadius, rotation })
		}
		this.transform.translate(width, 0).rotate(degToRad(90), 0, 0)
	}
}

export default Border
export { Radius, BoxRadius, BorderSide, BoxBorder }
