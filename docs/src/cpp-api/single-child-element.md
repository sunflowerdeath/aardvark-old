# `class SingleChildElement`

**Defined in:** [`"aardvark/element.hpp"`](@header/element.hpp)
<br>
**Extends:** [`Element`](@type/element)

## Members

```@toc
levels: [3]
```

---

### `SingleChildElement(std::shared_ptr<`[`Element`](@type/Element)`> child, bool is_repaint_boundary, bool size_depends_on_parent)`

**Parameters:**

- `std::shared_ptr<`[`Element`](@type/Element)`>` **child**
- `bool` **is_repaint_boundary** &ndash; Parameter passed to base [`Element`](@type/Element) class.
- `bool` **size_depends_on_parent** &ndash; Parameter passed to base [`Element`](@type/Element) class.

---

### `std::shared_ptr<`[`Element`](@type/Element)`> child`

---

### `void append_child(std::shared_ptr<`[`Element`](@type/Element)`> child)`

---

### `void remove_child(std::shared_ptr<`[`Element`](@type/Element)`> child)`

---

### `void visit_children(ElementChildrenVisitor visitor)`
