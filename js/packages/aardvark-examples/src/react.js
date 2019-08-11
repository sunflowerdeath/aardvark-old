import React from 'react'
import ReactDOMServer from 'react-dom/server'
import ReactAardvark from 'aardvark-react'

let Hello = () => React.createElement('div', null, 'Hello world!')
let res = ReactDOMServer.renderToString(React.createElement(Hello))
log(res)

let app = new DesktopApp()
let window = app.createWindow(640, 480)
let document = app.getDocument(window)

let App = () => {
	return (
		<Align
			align={{
				left: { type: 'abs', value: 50 },
				top: { type: 'abs', value: 100 }
			}}
		>
			<Sized
				sizeConstraints={{
					minWidth: { type: 'abs', value: 200 },
					maxWidth: { type: 'abs', value: 200 },
					minHeight: { type: 'abs', value: 50 },
					maxHeight: { type: 'abs', value: 50 }
				}}
			>
				<Background />
			</Sized>
		</Align>
	)
}

ReactAardvark.render(<App />, document)
