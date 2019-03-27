# `class elements::Align`

Element

**Defined in:** `aardvark/elements/align.hpp`
<br>
**Extends:** [`SingleChildElement`](@type/element)

## Members

```@toc
levels: [3]
```

---

### `Align(std::shared_ptr<Element> child, EdgeInsets insets, bool adjust_child = true, bool is_repaint_boundary = false)`

**Parameters:**

- `std::shared_ptr<Element>` **child**
- `EdgeInsets` **insets**
- `bool` **adjust_child** = true &ndash;
    Whether to reduce size of the child by the size of insets, or make it 
    equal to the size of the container. This is useful when you need to set 
    relative size and position at the same time.
- `bool` **is_repaint_boundary** = false

---

### `bool adjust_child`

---

### `EdgeInsets insets`
