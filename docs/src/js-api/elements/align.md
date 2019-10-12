# `class AlignElement`

Element that aligns its child inside the parent on absolute position or 
relative to the parent's size.

**Extends**: `Element`

## Example

```js
const align = new Align()
align.alignment = {
    left: { type: 'abs', value: 200 },
    top: { type: 'rel', value: 0.5 }
}
```

## Members

```@toc
levels: [3]
```

---

### `alignment`

Type: `Object`

Object describing the alignment with the following properties of type `Value`:

- `left`
- `top`
- `right`
- `bottom`

Default value is `Value.NONE`.

`left` and `top` has higher precedence, than `right` and `bottom`.

---

### `adjustChildSize`

Type: `boolean`
<br>
Default: `true`

Whether to reduce size of the child by the size of insets, or make it 
equal to the size of the container. This is useful when you need to set 
relative size and position at the same time.
