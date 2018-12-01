import Element from '../Element'
import BoxConstraints from '../BoxConstraints'
import Position from '../Position'
import Size from '../Size'

enum FlexDirection {
	Row,
	Column
}

enum FlexJustify {
	Start,
	End,
	Center,
	SpaceBetween,
	SpaceAround,
	SpaceEvenly
}

enum FlexAlign {
	Start,
	End,
	Center
}

interface FlexChild {
	align?: FlexAlign
	grow?: number
	size?: number
	element: Element<any>
}

interface FlexProps {
	direction: FlexDirection
	justify: FlexJustify
	align: FlexAlign
	children: Array<FlexChild>
}

class Flex extends Element<FlexProps> {
	layout(constraints: BoxConstraints) {
		const { direction, align, justify, children } = this.props
		const isRow = direction === FlexDirection.Row
		const [mainAxisName, crossAxisName] = isRow
			? ['left', 'top']
			: ['top', 'left']
		const [mainSizeName, crossSizeName] = isRow
			? ['width', 'height']
			: ['height', 'width']

		const flexChildren = []
		const fixedChildren = []
		let remainingSize = constraints[isRow ? 'maxWidth' : 'maxHeight']
		let totalGrow = 0
		for (let child of children) {
			if (child.grow !== undefined) {
				flexChildren.push(child)
				totalGrow += child.grow
			} else {
				fixedChildren.push(child)
				// use space by fixed children
				remainingSize -= child.size
			}
		}

		console.log('REMAINING SIZE', remainingSize)
		// if there is no flex children divide remaining size into spaces
		let spaceSize = 0
		if (flexChildren.length === 0) {
			if (justify === FlexJustify.SpaceBetween) {
				spaceSize = remainingSize / (children.length - 1)
			} else if (justify === FlexJustify.SpaceAround) {
				spaceSize = (remainingSize / children.length) / 2
			} else if (justify === FlexJustify.SpaceEvenly) {
				spaceSize = remainingSize / (children.length + 1)
			}
		}
		console.log('SPACE SIZE', spaceSize)

		// layout all children, find out max cross size
		const flexSize = remainingSize / totalGrow
		const childSizes = []
		let maxCrossSize = constraints[isRow ? 'minHeight' : 'minWidth']
		for (let child of children) {
			const mainChildSize =
				child.grow === undefined ? child.size : child.grow * flexSize
			const childConstraints = new BoxConstraints(
				isRow
					? {
							minWidth: mainChildSize,
							maxWidth: mainChildSize,
							minHeight: 0,
							maxHeight: constraints.maxHeight
					  }
					: {
							minWidth: 0,
							maxWidth: constraints.maxWidth,
							minHeight: mainChildSize,
							maxHeight: mainChildSize
					  }
			)
			console.log('C', childConstraints)
			const childSize = this.document.layoutElement(
				child.element,
				childConstraints
			)
			child.element.size = childSize
			maxCrossSize = Math.max(childSize[crossSizeName], maxCrossSize)
			childSizes.push(childSize)
		}

		// set positions of children
		let usedSize = 0
		if (justify === FlexJustify.SpaceEvenly) {
			usedSize += spaceSize
		} else if (justify === FlexJustify.Center) {
			let totalSize = 0
			for (const child of children) {
				totalSize += child.element.size[mainSizeName]
			}
			usedSize =
				(constraints[isRow ? 'maxWidth' : 'maxHeight'] - totalSize) / 2
		}
		for (let child of children) {
			if (justify === FlexJustify.SpaceAround) usedSize += spaceSize
			const childAlign = child.align || align
			const childCrossAxis =
				child.align === FlexAlign.Start
					? 0
					: child.align === FlexAlign.End
					? maxCrossSize - child.element.size[crossSizeName]
					: (maxCrossSize - child.element.size[crossSizeName]) / 2 // center
			child.element.relPosition = new Position(
				isRow
					? { left: usedSize, top: childCrossAxis }
					: { left: childCrossAxis, top: usedSize }
			)
			usedSize += child.element.size[mainSizeName]
			if (
				justify === FlexJustify.SpaceBetween ||
				justify === FlexJustify.SpaceAround ||
				justify === FlexJustify.SpaceEvenly
			) {
				usedSize += spaceSize
			}
		}

		return isRow
			? new Size({ width: constraints.maxWidth, height: maxCrossSize })
			: new Size({ width: maxCrossSize, height: constraints.maxHeight })
	}

	paint() {
		for (let child of this.props.children) {
			this.document.paintElement(child.element)
		}
	}
}

export { FlexAlign, FlexJustify, FlexDirection, FlexChild }
export default Flex
