import Element from '../Element'
import BoxConstraints from '../BoxConstraints'
import Size from '../Size'
import Position from '../Position'

interface StackProps {
	children: Array<Element<any>>
}

class Stack extends Element<StackProps> {
	layout(constraints: BoxConstraints) {
		for (let child of this.props.children) {
			child.size = this.document.layoutElement(child, constraints.makeLoose())
			child.relPosition = Position.origin
		}
		return new Size({
			width: constraints.maxWidth,
			height: constraints.maxHeight
		})
	}

	paint() {
		for (let child of this.props.children) {
			this.document.paintElement(child)
		}
	}
}

export default Stack
