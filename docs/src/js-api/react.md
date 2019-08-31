# JS API / React renderer

## Example

```js
import AardvarReact from '@advk/react'

const window = application.createWindow(640, 480)

const App = () => (
    <align alignment={new Alignment({
        left: Value.abs(100),
        top: Value.abs(100)
    })}>
        <sized sizeConstraints={SizeConstraints.exact({
            width: Value.abs(200),
            height: Value.abs(50)
        })}>
            <background color={new Color({ r: 255, g: 0, b: 0 })} />
        </sized>
    </align>
)

AardvarkReact.render(<App />, window.document)
```

## AardvarkReact

### `render(element: ReactElement, container: Document, callback?)`