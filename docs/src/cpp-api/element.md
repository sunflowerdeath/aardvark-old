# `class Element`

Base class for implementing elements.
Ancestor classes should implement several virtual methods
defining how element performs layout, paint and other stuff like hit testing

**Extends:** [`SingleChildElement`](T)

**Defined in:** [`"aardvark/inline_layout/text_span.hpp"`]()

## Members
- [`virtual std::string get_debug_name()`]()
- [`virtual Size layout(BoxConstraints constraints)`]()
- [`virtual void paint(bool is_changed)`]()
- [`virtual void visit_children(ElementChildrenVisitor visitor)`]()
- [`virtual bool hit_test(double left, double top)`]()
- [`virtual ResponderMode get_responder_mode()`]()
- [`virtual Responder* get_responder()`]()
- [`virtual void append_child(std::shared_ptr<Element> child)`]()
- [`virtual void remove_child(std::shared_ptr<Element> child)`]()
- [`virtual void insert_before_child(std::shared_ptr<Element>child)`]()
- [`void change()`]()
- [`bool is_parent_of(Element *elem)`]()
- [`Element* find_closest_relayout_boundary()`]()
- [`Element* find_closest_repaint_boundary()`]()

<hr>

### `virtual std::string get_debug_name()`

Returns name of the element. Used for debugging.

#### Return value:

`std::string` &ndash; Name of the element.

<hr>

### <code>virtual [Size](T) layout([BoxConstraints](T) constraints)</code>

Calculates size of the element size according to the provided constraints,
performs layout of children and sets their sizes and relative positions.

**Parameters:**

- [`BoxConstraints`]() **constraints** &ndash; Size constraints provided by the 
parent element.

**Return value:**

[`Size`]() &ndash; Element's size.

---

### `virtual void paint(bool is_changed)`

Paints element and its children.

#### Parameters:

- `bool` **is_changed**  &ndash;
 is `true` when the element itself
or some of its parents is changed.
When it is `false`, element is allowed to
reuse result of previous painting.

<hr>

### `virtual void visit_children(ElementChildrenVisitor visitor)`

Walks element's children in paint order.
`ElementChildrenVisitor` has the following type:
`std::function<void(std::shared_ptr<Element>)>`

---

### `virtual bool hit_test(double left, double top)`

Checks if element is hit by pointer. Default is checking element's box.

<hr>

### <code>bool is_parent_of([Element]()* elem)</code>

Checks whether this element is a parent or an ancestor of the given element.

#### Return value:

`bool` &ndash; `true` if element is a parent.


