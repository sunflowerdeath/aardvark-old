# JS / Elements / Sized

Element that sizes its child absolute or relative to the parent size.

## Example

Exact size:

```js
const sized = new Sized()
sized.size = {
    width: { type: 'abs', value: 200 },
    height: { type: 'abs', value: 50 }
}
```

Responsive column:

```js
const sized = new Sized()
sized.size = {
    width: { type: 'rel', value: 1 },
    maxWidth: { type: 'abs', value: 1000 }
}
```

```js
const sized = new Sized()
sized.size = {
    minHeight: Value.abs(200)
}
```

## Props
