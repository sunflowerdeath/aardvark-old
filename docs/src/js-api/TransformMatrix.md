# `TransformMatrix`

**Import:** `import { TransformMatrix } from '@advk/common'`

Object with helpers for creating transform matrixes.

## Members

```@toc
levels: [3]
```

---

### `identity`

An identity matrix.

Type: `Array`

---

### `makeTranslate(dx, dy)`

Makes translation matrix.

**Returns:** `Array`

---

### `makeScale(sx, sy)`

Creates scale matrix.

**Returns:** `Array`

---

### `makeRotate(angle, px, py)`

Creates rotation matrix.

**Arguments**:

- **angle**
  <br>
  Type: `float`
  <br>
  Rotation angle in radians.

- **px**
  <br>
  Type: `float`
  <br>
  X-coordinate of the rotation origin point.

- **py**
  <br>
  Type: `float`
  <br>
  Y-coordinate of the rotation origin point.

**Returns:** `Array`

---

### `append(a, b)`

Applies one transformation after another.

**Returns:** `Array`
