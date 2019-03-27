# Design / Inline layout

Inline layout system consists of two types of components:

- **Inline containers** &ndash; elements of a document that layout content in
lines.
- **Spans** &ndash; objects that represent content of different types, like plain 
text, rich text, inline decorations, inline blocks and others.

The system is designed in such a way that it is possible to implement new types
of containers and spans. For example, containers that layout content in columns
or pages, or wrap it around vector path.

Inline layout is similar to the document layout.
Span recieves constraints from parents and return its size.
Parent may be a container or another span, spans can be nested.
Difference is that in document layout there are only two possible results:
element can fit into constraints or not fit.
In inline layout the third type of result is possible - when part of the span
fits on the current line and remaining part can be wrapped to the next line.
In that case span splits itself into two new spans of the same type, container 
puts first span into current line and continues layout of the next line with 
the second part, which itself can split again.

## Span types 

- **TextSpan**

  Text with different styles.

- **DecorationSpan**

  Span that contains another spans and decorates them with background, border, 
  padding.

- **BlockSpan**

  Span that contains arbitrary element.

- **SpacingSpan**

  Empty space, for example, indent on the first line of a paragraph, 
  or before a list item.

- **ResponderSpan**

  Span that allows to handle events.

## Example

![inline-content](inline-painted.png)

```
<Paragraph>
    <TextSpan>Hello, my name is George!</TextSpan>
    <DecorationSpan {...}>
        <TextSpan>This text is inside decoration</TextSpan>
        <TextSpan style={...}>This text has different style</TextSpan>
        <BlockSpan size={...}>
            <Background color="..." />
        </BlockSpan>
    </DecorationSpan>
<Paragraph>
```

First, paragraph lays out spans into lines.
Spans, which are located on several lines, are divided into several parts.
After performing the layout, content has the following structure:

![inline-content](inline-lines.png)

Then these spans are rendered in the document.
`TextSpan` renders with `Text` element, and `DecorationSpan` renders with
`Border`, `Padding` and `Background` elements.

![inline-content](inline-render.png)
