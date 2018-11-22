import Element from './Element'
import LayerTree from './LayerTree'
import Layer from './Layer'
import ComposeOptions from './ComposeOptions'
import Position from './Position'
import Size from './Size'
import BoxConstraints from './BoxConstraints'
import Compositor from './Compositor'

// Add element to set ensuring that no element will be the children of another
const addQQ = (set: Set<Element<any>>, added: Element<any>) => {
	for (const elem of set) {
		if (elem.isParentOf(added)) return // Parent is already in the set
		if (added.isParentOf(elem)) set.delete(elem) // Child is in the set
	}
	set.add(added)
}

class Document {
	compositor: Compositor
	screen: Layer
	root: Element<any>
	changedElements: Set<Element<any>> = new Set<Element<any>>()
	isInitialPaint: boolean

	constructor({ compositor, root }: { compositor: Compositor, root: Element<any> }) {
		this.compositor = compositor
		this.screen = compositor.getScreenLayer()
		this.setRoot(root)
	}
	
	changeElement(elem: Element<any>) {
		addQQ(this.changedElements, elem)
	}

	setRoot(newRoot: Element<any>) {
		this.root = newRoot
		this.root.isRepaintBoundary = true
		this.root.relPosition = new Position({ top: 0, left: 0 })
		this.root.size = this.screen.size
		this.isInitialPaint = true
	}

	/*
	resize(size: Size) {
		this.size = size
		this.isInitialPaint = true
	}
	*/

	paint() {
		if (this.isInitialPaint) {
			this.initialPaint()
		} else {
			this.repaint()
		}
	}
	
	private initialPaint() {
		this.layoutElement(this.root, BoxConstraints.fromSize(this.screen.size))
		this.paintElement(this.root, true)
		this.composeLayers()
		this.isInitialPaint = false
	}
	
	private repaint(): void {
		if (this.changedElements.size === 0) return // nothing to repaint
		const relayoutBoundaries = new Set<Element<any>>()
		for (let elem of this.changedElements) {
			addQQ(relayoutBoundaries, elem.findClosestRelayoutBoundary())
		}
		const repaintBoundaries = new Set<Element<any>>()
		for (const elem of relayoutBoundaries) {
			this.layoutElement(elem, elem.prevConstraints)
			addQQ(repaintBoundaries, elem.findClosestRepaintBoundary())
		}
		for (const elem of repaintBoundaries) this.paintElement(elem, true)
		this.composeLayers()
		this.changedElements.clear()
	}
	
	currentElement: Element<any>
	// Layer tree of the current repaint boundary element
	private currentLayerTree: LayerTree
	// Previous layer tree of the current repaint boundary element
	private prevLayerTree: LayerTree
	// Layer that is currently used for painting
	private currentLayer: Layer

	layoutElement(elem: Element<any>, constraints: BoxConstraints): Size {
		console.log("Layout element", elem)
		elem.document = this
		elem.isRelayoutBoundary = 
			BoxConstraints.isTight(constraints) || !elem.sizedByParent
		const size = elem.layout(constraints)
		elem.prevConstraints = constraints
		return size
	}
	
	paintElement(elem: Element<any>, isRepaintRoot?: boolean): void {
		console.log("Paint element", elem)
		if (!isRepaintRoot) elem.parent = this.currentElement
		this.currentElement = elem
		if (elem.isRepaintBoundary) {
			// Save previous tree to be able to reuse layers from it
			this.prevLayerTree = elem.layerTree
			// Create new tree and add it to the parent tree
			const parentTree = isRepaintRoot ?
				(this.isInitialPaint ? undefined : this.prevLayerTree.parent) :
				this.currentLayerTree
			elem.layerTree = new LayerTree(elem)
			elem.layerTree.parent = parentTree
			if (parentTree) {
				if (isRepaintRoot) {
					parentTree.replace(this.prevLayerTree, elem.layerTree)
				} else {
					parentTree.add(elem.layerTree)
				}
			}
			// Make new tree current
			this.currentLayerTree = elem.layerTree
			this.currentLayer = undefined
		}
		elem.absPosition = elem.parent
			? Position.add(elem.parent.absPosition, elem.relPosition)
			: elem.relPosition
		elem.paint()
		if (elem.isRepaintBoundary) {
			this.prevLayerTree = undefined
			this.currentLayerTree = this.currentLayerTree.parent
			this.currentLayer = undefined
		}
		this.currentElement = elem.parent
	}

	getLayer() {
		// If there is no current layer, setup default layer
		if (!this.currentLayer) {
			this.createLayer(this.currentLayerTree.element.size)
		}
		return this.currentLayer
	}

	// Creates layer and adds it to the current layer tree, reusing layers from
	// previous repaint if possible.
	createLayer(size: Size) {
		let layer = this.prevLayerTree && this.prevLayerTree.findBySize(size)
		if (layer) {
			// Reuse layer
			this.prevLayerTree.remove(layer)
			layer.reset()
		} else {
			// Create new layer
			layer = this.compositor.createOffscreenLayer(size)
		}
		this.currentLayerTree.add(layer)
		this.currentLayer = layer
		return layer
	}
	
	composeLayers() {
		this.screen.clear()
		this.paintLayerTree(this.root.layerTree)
	}

	paintLayerTree(tree: LayerTree) {
		for (let item of tree.children) {
			if (item instanceof Layer) {
				this.compositor.paintLayer(item, tree.element.absPosition)
			} else {
				this.paintLayerTree(item)
			}
		}
	}
}

export default Document
