import React from 'react'
import ReactDOMServer from 'react-dom/server'
import ReactAardvark from 'aardvark-react'

let app = new DesktopApp()
let window = app.createWindow(640, 480)
let document = app.getDocument(window)

let App = () => {
    log('render app')
	return (
		<align
			align={{
				left: { type: 'abs', value: 50 },
				top: { type: 'abs', value: 100 }
			}}
		>
			<sized
				sizeConstraints={{
					minWidth: { type: 'abs', value: 200 },
					maxWidth: { type: 'abs', value: 200 },
					minHeight: { type: 'abs', value: 50 },
					maxHeight: { type: 'abs', value: 50 }
				}}
			>
				<background />
			</sized>
		</align>
	)
}

ReactAardvark.render(<App />, document)

app.run()
