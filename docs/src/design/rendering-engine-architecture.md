# Design / Rendering engine architecture

This document provides high level overview of the rendering engine architecture
and describes some implementation details.

Rendering consists of three phases: layout, painting and compositing.

## Layout

In the layout phase document calculates size and position of all elements.
Each element takes constraints &ndash; minimum and maximum possible size &ndash;
and decides what size it wants to be. While doing it, element also calls layout
of its children, and sets their sizes and positions.

Performing full document relayout on each change is bad for perfomance.
Document can determine what part may be affected by changing a certain element
and perform relayout only for this part.

For relayout document finds closest element, that isolates changes.
It means that no changes inside that element can change layout outside of it.
Such elements are called "relayout boundaries".
Here's how document determines if an element is a relayout boundary:

- It gets tight constraints from the parent, so it can only be one size

- Its layout depends only on the incoming constraints, and not on
  properties or content.
  For example, when element always expands to all available space.

## Painting

Painting &ndash; creating visual representation of elements.
During painting phase, elements execute painting commands to display themselves
and call painting of its children.
Before performing a set of painting commands, element should acquire surface,
because it can be changed, if some children are painted on separate layers.

Only elements that were changed directly or changed their size or position 
after relayout will update their visual representation.
All other elements will look the same, so they can reuse result of previous
painting. This can be done in two ways:

- Record painting commands into the object, that can be replayed later.

- Painting element onto separate surface and copying it from it. This is called
  rasterization. It is fastest, but it requires additional memory.

## Layers system

Support for layers is useful for several reasons:

- It allows to repaint not all layers, but only those on which changed elements
  are located.

- Operations with layers, like translating, rotating or changing opacity can be 
  performed without repainting, it requires only compositing. Compositing is fast
  because it is performed on GPU.

- Some layers come from external sources, for example, camera, video or native
  platform view.

It is important to note, that if some element uses a separate layer, then
everything that is painted after it should be placed on another separate layer.
So the element can affect what layers the elements will use in a completely
different part of the document. 
Because of this, it is not clear what needs to be repainted.

To solve this problem, you can use special elements that restrict layers inside.
They are called repaint boundaries. Such elements can be repainted separately.
Using the repaint boundary may require more layers than without it, thats why
you need to specify them manually, not automatically like relayout boundaries.

This is an example how layers change after adding repaint boundary.
This is tree of elements. Сolor indicates on which layer the element is painted.
Element can have more that one color, because it can be painted on several layers -
before and after painting its children.
Red line shows in which order the elements are painted.

Without repaint boundary:

![before](layers.png)

After adding  repaint boundary:

![after](layers-repaint-boundary.png)

## Clipping

When parent paints its child, it can optionally provide clipping path.
This clip becomes current. If there was clip already, new current clip
is intersection of both paths. Since layers can change transform without
repainting, they must be painted without clipping, and clipping must be applied
when composing this layer.

Clipping is antialiased. This means that pixels near the edge of clip path
can be painted semitransparent to make edge look more smooth.
This may cause artifacts when several colors are painted on top of each other.
Without clip second color fully cover first color, but with clipping
semitransparent pixel of the second color will be painted on top of
semitransparent pixel of the first color and both colors will be mixed.
Artifacts can be avoided by painting clipped elements on separate layer, but
creating additional layers cost performance.

| clipping artifacts                             | clipping with layer                    |
| ---------------------------------------------- | -------------------------------------- |
| ![clipping with artifacts](clip-artifacts.png) | ![clipping with layer](clip-layer.png) |

<br>

```js
<FixedSize width={100} height={100}>
  <Clip path={circle}>
    <Background color="red" />
    <Align left={value.abs(50)} top={value.abs(50)}>
      <Background color="blue" />
    </Align>
  </Clip>
</FixedSize>
```

```js
<FixedSize width={100} height={100}>
  <Clip path={circle}>
    <Layer>
      <Background color="red" />
      <Align left={value.abs(50)} top={value.abs(50)}>
        <Background color="blue" />
      </Align>
    </Layer>
  </Clip>
</FixedSize>
```
