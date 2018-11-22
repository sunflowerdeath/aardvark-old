import Document from './Document'
import BoxConstraints from './BoxConstraints'
import Size from './Size'
import Position from './Position'
import LayerTree from './LayerTree'

type Partial<T> = {
    [P in keyof T]?: T[P]
}

interface ElementOptions {
	isRepaintBoundary?: boolean
}

abstract class Element<P> {
	document: Document
	parent: Element<any>
	// These props are set by parent layout functions
	size: Size
	relPosition: Position
	// Absolute position is calculated after layout painting
	absPosition: Position
	// Size depends only on input constraints
	sizedByParent: boolean = true
	// When element is relayout boundary, changes inside it do not affect 
	// layout of parents. This can happen when element recieves tight
	// constraints, so it is always same size, or when element's size
	// depends only on input constraints.
	// size of its children for calculating own size.
	// This prop is set automatically depending on the given layout constraints
	// and value of the `sizeDependsOnChildren`.
	isRelayoutBoundary: boolean
	// This is used for relayout
	prevConstraints: BoxConstraints
	// Element can be repaint boundary when it does not share layers  with
	// another elements. This ensures that it can be repainted safely, if all
	// changes are contained within it.
	// This prop should be set explicitly.
	isRepaintBoundary: boolean = false
	layerTree: LayerTree
	props: P

	constructor(props: P, options?: ElementOptions) {
		this.props = props
		if (options) {
			this.isRepaintBoundary = options.isRepaintBoundary
		}
	}

	setProps(props: Partial<P>) {
		Object.assign(this.props, props)
		this.document.changeElement(this)
	}

	abstract layout(constraints: BoxConstraints): Size

	abstract paint(): void

	isParentOf(elem: Element<any>): boolean {
		let current = elem.parent
		while (current) {
			if (current === this) return true
		}
		return false
	}

	findClosestRepaintBoundary(): Element<any> {
		let current: Element<any> = this
		while (!current.isRepaintBoundary) current = current.parent
		return current
	}

	findClosestRelayoutBoundary(): Element<any> {
		let current: Element<any> = this
		while (!current.isRelayoutBoundary) current = current.parent
		return current
	}
}

export default Element
