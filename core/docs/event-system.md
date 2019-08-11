# Event system

Document receives pointer events from the window.

First, it determines which elements are positioned under the pointer.
Then it iterates through these elements from top to bottom and calls their event
handlers.

When iterating, it takes into account elements hit test mode. 
It can be one of the following:

- `PassThrough` - After element handles event, it passes it to the element 
  that is behind.

- `PassToParent` - Passes event to the parent element (or any further ancestor)
  that is behind this element.

- `Absorb` - Does not pass event after handling.
