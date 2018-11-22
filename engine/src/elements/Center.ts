import BoxConstraints from '../BoxConstraints'
import Position from '../Position'
import Size from '../Size'
import Element from '../Element'

interface CenterProps {
	child: Element<any>
}

class Center extends Element<CenterProps> {
	sizedByParent = false

	layout(constraints: BoxConstraints) {
		const { child } = this.props
		const childConstraints = new BoxConstraints({
			minWidth: 0,
			maxWidth: constraints.maxWidth,
			minHeight: 0,
			maxHeight: 0
		})
		const childSize = this.document.layoutElement(child, childConstraints)
		child.size = childSize
		child.relPosition = new Position({
			left: (constraints.maxWidth - childSize.width) / 2,
			top: (constraints.maxHeight - childSize.height) / 2
		})
		return new Size({
			width: constraints.maxWidth,
			height: constraints.maxHeight
		})
	}
	
	paint() {
		this.document.paintElement(this.props.child)
	}
}

export default Center
