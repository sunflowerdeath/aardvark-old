# JS / React / Scrollable

Component that allows scrolling content with keyboard, mouse and touch.

## Props

**mousewheelScrollSpeed**
<br>
type: `number`
<br>
default: `100`

**keyboardScrollSpeed**
<br>
type: `number`
<br>
default: `50`

**dragOverscrollResistance**
<br>
type: `(overscrollValue: number) => number`
<br>
default: `OverscrollResistance.diminishing`

- `none`
- `full`
- `diminishing`

**dragDecayDeceleration**
<br>
type: `number`
<br>
default: `0.998`

**dragOverscrollBounciness**
<br>
type: `number`
<br>
default: `6`

**contentWrapper**
<br>
type: `Component`

Component for scrollbar or overscroll indicator

Component recieves following props:
- `children`
- `scroll`
- `overscroll`
- `scrollHeight`


