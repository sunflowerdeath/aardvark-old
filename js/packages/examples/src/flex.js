import React, { useState, useEffect, useRef } from 'react'
import ReactAardvark from '@advk/react-renderer'
import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from '@advk/common'

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

const COLOR_RED = { alpha: 255, red: 0, green: 255, blue: 0 }
const COLOR_GREEN = { alpha: 255, red: 255, green: 0, blue: 0 }
const COLOR_BLUE = { alpha: 255, red: 0, green: 0, blue: 255 }

const Value = {
	abs(value) {
		return { type: 'abs', value }
	},

	rel(value) {
		return { type: 'rel', value }
	},

	none() {
		return { type: 'none', value: 0 }
	}
}

const FlexJustify = {
	start: 0,
	center: 1,
	end: 2,
	spaceAround: 3,
	spaceBetween: 4,
	spaceEvenly: 5
}

const Box = ({ color }) => (
	<sized sizeConstraints={{ width: Value.abs(50), height: Value.abs(50) }}>
		<background color={color} />
	</sized>
)

const Container = ({ left, top, children }) => (
	<align align={{ left: Value.abs(left), top: Value.abs(top) }}>
		<sized
			sizeConstraints={{
				width: Value.abs(300),
				height: Value.abs(100)
			}}
		>
			{children}
		</sized>
	</align>
)

const App = () => {
	return (
		<stack>
			<Container left={20} top={20}>
				<flex>
					<Box color={COLOR_RED} />
					<Box color={COLOR_GREEN} />
					<Box color={COLOR_BLUE} />
				</flex>
			</Container>
			<Container left={20} top={140}>
				<flex justify={FlexJustify.spaceEvenly}>
					<Box color={COLOR_RED} />
					<Box color={COLOR_GREEN} />
					<Box color={COLOR_BLUE} />
				</flex>
			</Container>
		</stack>
	)
}

ReactAardvark.render(<App />, document)
