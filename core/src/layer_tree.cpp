#include "layer_tree.hpp"

namespace aardvark {

LayerTree::LayerTree(Element* element) {
    this->element = element;
    transform.reset();
};

void LayerTree::add(LayerTreeNode item) {
    children.push_back(item);
    if (auto tree = std::get_if<LayerTree*>(&item)) (*tree)->parent = this;
};

/*
void LayerTree::remove(LayerTreeNode item) {
    auto it = std::find(children.begin(), children.end(), item);
    if (auto tree = std::get_if<LayerTree>(&*it)) tree->parent = nullptr;
    children.erase(it);
}

void LayerTree::set_parent(LayerTree* new_parent) {
    if (new_parent != parent) {
        if (parent != nullptr) parent->remove(this);
		if (new_parent != nullptr) new_parent->add(this);
	}
	parent = new_parent;
}
*/


}  // namespace aardvark
