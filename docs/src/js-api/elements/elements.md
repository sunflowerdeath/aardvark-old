# JS / qwe

**Contents**

```@toc
```

---

## `class Element`

Base class for all elements.

### `appendChild(child)`

### `insertChildBefore(child, beforeChild)`

### `removeChild(child)`

### `width`
Type: `number`

### `height`

### `left`

### `top`

### `parent`

---

## `createElement(type, props)`

Creates an element with specified properties.
Validates types of props.

**Arguments:**

- **type**
  <br>
  Type: `string`
  
  Type of the element.
  
- **props**
  <br>
  Type: `object`
  
  Properties for the element.

**Returned value:**

[`Element`]() - Created element

---

## `createElementWithChildren(element, props, children)`

Creates an element with specified properties and children.
Validates types of props and also checks that element gets correct number of
children.

**Arguments:**

- **type**
  <br>
  Type: `string`
  
  Type of the element.
  
- **props**
  <br>
  Type: `object`
  
  Props of the element.
  
- **children**
  <br>
  Type: [`Element`]()` | Array<`[`Element`]()`> | undefined`

**Returned value:**

[`Element`]() - Created element

---

## `updateElementProps(element, newProps, oldProps?)`

Updates props of the element.
When old props are provided, it updates only changed props.

**element**
<br>
Type: [`Element`]()

Type of the element.

**props**
<br>
Type: `object`

Props of the element.

**children**
<br>
Type: `Element | Array<Element> | undefined`
