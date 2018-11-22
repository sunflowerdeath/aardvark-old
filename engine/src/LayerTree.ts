import Layer from './Layer'
import Element from './Element'
import Size from './Size'

class LayerTree {
	// Owner of the layer tree
	element: Element<any>

	// Parent layer tree
	parent: LayerTree

	// Child layers and trees
	children: Array<LayerTree|Layer> = []

	constructor(elem: Element<any>) {
		this.element = elem
	}

	add(item: LayerTree|Layer) {
		this.children.push(item)
	}

	replace(oldItem: LayerTree|Layer, newItem: LayerTree|Layer) {
		const index = this.children.indexOf(oldItem)
		this.children[index] = newItem
	}

	remove(item: LayerTree|Layer) {
		const index = this.children.indexOf(item)
		this.children.splice(index, 1)
	}

	findBySize(size: Size): Layer|undefined {
		for (const item of this.children) {
			if (item instanceof Layer && Size.isEqual(item.size, size)) {
				return item
			}
		}
	}
}

export default LayerTree
