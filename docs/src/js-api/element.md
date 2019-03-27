# `class Element`

Base class for all elements.
Ancestor classes should implement several virtual methods
defining how element performs layout, paints itself and other stuff like
hit testing and gesture responding.

**Defined in:** [`"aardvark/element.hpp"`](@header/element.hpp)

## Members

```@toc
levels: [3]
```

---

### `virtual std::string get_debug_name()`

Returns name of the element. Used for debugging.

#### Return value:

`std::string` &ndash; Name of the element.

---

### <code>virtual [Size](T) layout([BoxConstraints](T) constraints)</code>

Calculates size of the element size according to the provided constraints,
performs layout of children and sets their sizes and relative positions.

**Parameters:**

- [`BoxConstraints`](@type/BoxConstraints) **constraints** &ndash; Size
constraints provided by the parent element.

**Return value:**

[`Size`](@type/Size) &ndash; Element's size.

---

### `virtual void paint(bool is_changed)`

Paints element and its children.

#### Parameters:

- `bool` **is_changed**  &ndash;
 is `true` when the element itself
or some of its parents is changed.
When it is `false`, element is allowed to
reuse result of previous painting.

---

### `virtual void visit_children(ElementChildrenVisitor visitor)`

Walks element's children in paint order.
`ElementChildrenVisitor` has the following type:
`std::function<void(std::shared_ptr<Element>)>`

---

### `virtual bool hit_test(double left, double top)`

Checks if element is hit by pointer. Default implementation checks element's box.

---

### <code>bool is_parent_of([Element](@type/Element)* elem)</code>

Checks whether this element is a parent or an ancestor of the given element.

#### Return value:

`bool` &ndash; `true` if element is a parent.


