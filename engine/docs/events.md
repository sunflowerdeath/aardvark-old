## Event system

Application object dipatches events from windows to corresponding documents.

Then document determines which elements should handle this event and call
their handlers.

First, it performs hit testing - detecting which elements are positioned under
pointer.
Then it iterates through stack of hit elements from top to bottom, and 
  

## Hit testing

## Gesture responding

- PassThrough

  After element handles event, it passes it to the element that is behind.

- PassToParent

  Passes event to the parent element (or any further ancestor) behind this 

  element. This is default mode.

- Absorb

  Does not pass event after handling.

- Capture

  Makes this element handle events exclusively and even when it is no longer

  hit by pointer. Element continues to handle event until it stops capturing

  or is removed from the document.