import BoxConstraints from '../BoxConstraints'
import Element from '../Element'
import Size from '../Size'
import Position from '../Position'

interface FixedSizeProps {
	size: Size,
	child: Element<any>
}

class FixedSize extends Element<FixedSizeProps> {
	props: FixedSizeProps

	layout() {
		const { size, child } = this.props
		this.document.layoutElement(child, BoxConstraints.fromSize(size))
		child.size = size
		child.relPosition = new Position({ left: 0, top: 0 })
		return size
	}
	
	paint() {
		this.document.paintElement(this.props.child)
	}
}

export default FixedSize
