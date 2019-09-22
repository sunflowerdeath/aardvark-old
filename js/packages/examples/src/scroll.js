import React, { useState, useCallback } from 'react'
import ReactAardvark from '@advk/react-renderer'
import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from '@advk/common'

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

const Value = {
	abs(value) {
		return { type: 'abs', value }
	},

	rel(value) {
		return { type: 'rel', value }
	},

	none: { type: 'none', value: 0 }
}

const Color = {
	WHITE: { alpha: 255, red: 255, green: 255, blue: 255 },
	BLACK: { alpha: 255, red: 0, green: 0, blue: 0 },
	LIGHTGRAY: { alpha: 255, red: 220, green: 220, blue: 220 },
	RED: { alpha: 255, red: 255, green: 0, blue: 0 },
	GREEN: { alpha: 255, red: 0, green: 255, blue: 0 },
	BLUE: { alpha: 255, red: 0, green: 0, blue: 255 }
}

const v0 = { width: Value.abs(50), height: Value.abs(50) }

const Box = ({ color }) => (
	<sized sizeConstraints={v0}>
		<background color={color} />
	</sized>
)

const Label = ({ children }) => (
	<flexChild align={FlexAlign.center}>
		<sized sizeConstraints={{ width: Value.abs(100) }}>
			<text text={children} />
		</sized>
	</flexChild>
)

const isLeftMouseButtonUp = event =>
	event.tool === PointerEventTool.MOUSE &&
	event.action === PointerEventAction.BUTTON_RELEASE // &&

const v1 = { left: Value.abs(30), top: Value.abs(30) }
const v2 = {
						width: Value.abs(200),
						height: Value.abs(200)
					}
                    const v3 = { right: Value.abs(30), top: Value.abs(30) }
                    const v4 = {
						width: Value.abs(50),
						height: Value.abs(50)
					}

const App = () => {
	const [scrollTop, setScrollTop] = useState(0)
	return (
		<stack>
			<background color={Color.WHITE} />
			<align align={v1}>
				<sized
					sizeConstraints={v2}
				>
					<stack>
						<background color={Color.LIGHTGRAY} />
						<scroll scrollTop={scrollTop}>
							<Box color={Color.RED} />
							<Box color={Color.GREEN} />
							<Box color={Color.BLUE} />
							<Box color={Color.RED} />
							<Box color={Color.GREEN} />
							<Box color={Color.BLUE} />
							<Box color={Color.RED} />
							<Box color={Color.GREEN} />
							<Box color={Color.BLUE} />
						</scroll>
					</stack>
				</sized>
			</align>
			<align align={v3}>
				<sized
					sizeConstraints={v4}
				>
					<responder
						handler={useCallback(event => {
							if (isLeftMouseButtonUp(event)) {
								setScrollTop(scrollTop => scrollTop + 10)
							}
						}, [setScrollTop])}
					>
						<background color={Color.RED} />
					</responder>
				</sized>
			</align>
		</stack>
	)
}

ReactAardvark.render(<App />, document)
