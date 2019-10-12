# `class Element`

Base class for all elements.

## Members

```@toc
levels: [3]
```

---

### `document`
type: `Document`

Document

---

### `children`
type: `Array<Element>`

---

### `parent`
type: `Element|null`

Parent element.

---

### `width`
Type: `number`

Width of the element.

---

### `height`

Height of the element.

---

### `left`

---

### `top`

---

### `appendChild(child)`

Appends child to the element.
When element is a single child and already has a child, new child replaces previous.

---

### `insertChildBefore(child, beforeChild)`

Inserts child before another child.

---

### `removeChild(child)`

Removes child from the element.
