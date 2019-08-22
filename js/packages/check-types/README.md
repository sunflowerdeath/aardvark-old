# Check-types

Simple library for checking types and obtaining error messages.

It is similar to `prop-types`. Differences are:

- It is intended for checking types in production, not just for development.
- Direct calling of checkers is allowed.
- All values are required by default, to make property optional - use `optional`.
- Has some differences in checks names (`oneOf -> enum`, `oneOfType -> union`, 
  `exact -> shape`, `shape -> looseShape`) and slightly different error messages.
- Not ensures compatibility with older browsers.

## Usage

```js
import check from '@advk/check-types'

const err1 = check.number(1, 'propName', 'Location') // undefined

const err2 = check.number('a', 'propName', 'Location')
// [CheckError: Invalid property `propName.a` of type `string` supplied to 
// Location, expected `number`.]

const checker = check.shape({
    a: check.number,
    b: check.optional(check.string)
})
const err3 = checker({ a: 'a' }, null, 'Location')
// [CheckError: Invalid property `a` of type `string` supplied to Location, 
// expected `number`.]
```
