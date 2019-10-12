# `class SizedElement`

Element that sizes its child absolute or relative to the parent size.

**Extends**: `Element`

## Example

Exact size:

```js
const sized = new Sized()
sized.sizeConstraints = {
    width: Value.abs(200),
    height: Value.abs(50)
}
```

Responsive column:

```js
const sized = new Sized()
sized.sizeConstraints = {
    width: Value.rel(1),
    maxWidth: Value.abs(1000)
}
```

---

### sizeConstraints

Type: `object`

Object describing size constraints with the following properties of type `Value`:

- `width`
- `height`
- `minWidth`
- `maxWidth`
- `minHeight`
- `maxHeight`

Default values are `Value.NONE`.
When `width` or `height` are provided, they are used as base size.
Then it is constrained from top and bottom by `min` and `max` sizes.

Sized element always respects provided constraints, even if it can not make
child be that size. For example, when you try to size not resizeable element, 
like text span.
