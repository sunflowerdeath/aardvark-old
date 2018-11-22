import Element from '../Element'
import BoxConstraints from '../BoxConstraints'
import Size from '../Size'
import Position from '../Position'

class Alignment {
	value: number
	isRelative: boolean

	constructor(value: number, isRelative: boolean) {
		this.value = value
		this.isRelative = isRelative
	}

	calc(total: number) {
		return this.isRelative ? this.value * total : this.value
	}
}

interface AlignProps {
	child: Element<any>,
	top?: Alignment,
	left?: Alignment,
	bottom?: Alignment,
	right?: Alignment
}

class Align extends Element<AlignProps> {
	sizedByParent = true

	layout(constraints: BoxConstraints): Size {
		const { top, left, bottom, right, child } = this.props
		const horizAlign = (left || right).calc(constraints.maxWidth)
		const vertAlign = (top || bottom).calc(constraints.maxHeight)
		const childConstraints = new BoxConstraints({
			minWidth: 0,
			maxWidth: constraints.maxWidth - horizAlign,
			minHeight: 0,
			maxHeight: constraints.maxHeight - vertAlign
		})
		const size = this.document.layoutElement(child, childConstraints)
		child.size = size
		child.relPosition = new Position({
			left: left ? horizAlign : 0,
			top: top ? vertAlign : 0
		})
		return new Size({
			width: constraints.maxWidth,
			height: constraints.maxHeight
		})
	}

	paint() {
		this.document.paintElement(this.props.child)
	}

	static px(val: number) {
		return new Alignment(val, false)
	}

	static rel(val: number) {
		return new Alignment(val, true)
	}
}

export default Align
