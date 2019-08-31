# JS / Elements / Align

Element that positions its child inside the parent on absolute or relative position.

## Example

```js
const align = new Align()
align.alignment = {
    left: { type: 'abs', value: 200 },
    top: { type: 'rel', value: 0.5 }
}
```

```js
const padding = { type: 'abs', value: 10 }
const align = new Align()
align.alignment = {
    left: padding,
    top: padding,
    right: padding,
    bottom: padding
}
```

## Props

**alignment**
<br>
type: `Object`

Configuration of the alignment.
Alignment is an object with the following properties:

- **left** `Value` – qwe. Default is `Value.NONE`.
- **top** `Value` – qwe
- **right** `Value` – qwe
- **bottom** `Value` – qwe

**adjustChildSize**
<br>
Type: `boolean`
<br>
Default: `true`

Whether to reduce size of the child by the size of insets, or make it 
equal to the size of the container. This is useful when you need to set 
relative size and position at the same time.
