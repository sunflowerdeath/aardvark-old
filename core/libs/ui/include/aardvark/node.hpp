#pragma once

#include <functional>
#include <memory>

#define NODE_PROP(TYPE, NAME) \
    TYPE NAME;                   \
    void set_##NAME(TYPE& val) { \
        NAME = val;              \
        change();                \
    };

#define NODE_PROP_DEFAULT(TYPE, NAME, DEFAULT) \
    TYPE NAME = DEFAULT;                          \
    void set_##NAME(TYPE& val) {                  \
        NAME = val;                               \
        change();                                 \
    };

namespace aardvark {

template <typename T>
using NodeChildrenVisitor = std::function<void(std::shared_ptr<T>&)>;

template <typename T, typename OwnerT>
using NodeChangeFunc = std::function<void(OwnerT*, T*)>;

template <typename T, typename OwnerT>
class Node {
  public:
    Node(NodeChangeFunc<T, OwnerT> change_fn = nullptr)
        : change_fn(change_fn) {}

    virtual void append_child(std::shared_ptr<T> child) {}
    virtual void insert_before_child(
        std::shared_ptr<T> child, std::shared_ptr<T> before_child) {}
    virtual void remove_child(std::shared_ptr<T> child) {}
    virtual void visit_children(NodeChildrenVisitor<T> visitor) {}
    virtual int get_children_count() { return 0; }
    virtual std::shared_ptr<T> get_child_at(int index) { return nullptr; }

    void change() {
        if (change_fn && owner != nullptr) change_fn(owner, node_from_this());
    }

    bool is_parent_of(T* elem) {
        auto current = elem->parent;
        while (current) {
            if (current == this) return true;
            current = current->parent;
        }
        return false;
    }

    void set_owner(OwnerT* owner) {
        if (this->owner == owner) return;
        this->owner = owner;
        visit_children([this, owner](std::shared_ptr<T>& child) {
            child->set_owner(owner);
        });
    }

    OwnerT* owner = nullptr;
    T* parent = nullptr;

  private:
    NodeChangeFunc<T, OwnerT> change_fn;
    T* node_from_this() { return reinterpret_cast<T*>(this); }
};

template <typename T, typename OwnerT>
class SingleChildNode : public virtual Node<T, OwnerT> {
  public:
    SingleChildNode(
        std::shared_ptr<T> child, NodeChangeFunc<T, OwnerT> change_fn = nullptr)
        : child(child), Node<T, OwnerT>(change_fn) {
        if (child) child->parent = dynamic_cast<T*>(this);
    }

    void remove_child(std::shared_ptr<T> child) override {
        if (this->child == child) {
            // Change is called before removing, so the owner can check if this
            // changed element is parent of another one
            this->change();
            this->child->parent = nullptr;
            this->child->set_owner(nullptr);
            this->child = nullptr;
        }
    }

    void append_child(std::shared_ptr<T> child) override {
        if (child->parent != nullptr) child->parent->remove_child(child);
        this->child = child;
        this->child->set_owner(this->owner);
        this->child->parent = dynamic_cast<T*>(this);
        this->change();
    }

    void visit_children(NodeChildrenVisitor<T> visitor) override {
        if (child != nullptr) visitor(child);
    }

    int get_children_count() override { return child == nullptr ? 0 : 1; }

    std::shared_ptr<T> get_child_at(int index) override {
        if (index == 0 && child != nullptr) return child;
        return nullptr;
    }

    std::shared_ptr<T> child = nullptr;
};

template <typename T, typename OwnerT>
class MultipleChildrenNode : public virtual Node<T, OwnerT> {
  public:
    MultipleChildrenNode(
        std::vector<std::shared_ptr<T>> children,
        NodeChangeFunc<T, OwnerT> change_fn)
        : Node<T, OwnerT>(change_fn), children(children){};

    void append_child(std::shared_ptr<T> child) override {
        if (child->parent != nullptr) child->parent->remove_child(child);
        child->parent = dynamic_cast<T*>(this);
        child->set_owner(this->owner);
        children.push_back(child);
        this->change();
    }

    void remove_child(std::shared_ptr<T> child) override {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            this->change();
            child->parent = nullptr;
            child->set_owner(nullptr);
            children.erase(it);
        }
    }

    void insert_before_child(
        std::shared_ptr<T> child, std::shared_ptr<T> before_child) override {
        if (child->parent != nullptr) child->parent->remove_child(child);
        auto it = std::find(children.begin(), children.end(), before_child);
        if (it == children.end()) return;
        child->parent = dynamic_cast<T*>(this);
        child->set_owner(this->owner);
        children.insert(it, child);
        this->change();
    }

    void visit_children(NodeChildrenVisitor<T> visitor) override {
        for (auto& child : children) visitor(child);
    };

    int get_children_count() override { return children.size(); };

    std::shared_ptr<T> get_child_at(int index) override {
        return (index + 1 > children.size()) ? nullptr : children[index];
    };

    std::vector<std::shared_ptr<T>> children;
};

}  // namespace aardvark
