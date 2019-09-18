import React, { useState, useEffect, useRef } from 'react'
import ReactAardvark from '@advk/react-renderer'
import {
	GestureResolver,
	PointerEventAction,
	PointerEventTool
} from '@advk/common'

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

const Color = {
	WHITE: { alpha: 255, red: 255, green: 255, blue: 255 },
	LIGHTGRAY: { alpha: 255, red: 220, green: 220, blue: 220 },
	RED: { alpha: 255, red: 0, green: 255, blue: 0 },
	GREEN: { alpha: 255, red: 255, green: 0, blue: 0 },
	BLUE: { alpha: 255, red: 0, green: 0, blue: 255 }
}

const COLOR_ACTIVE = { alpha: 255, red: 66, green: 165, blue: 244 }

const Value = {
	abs(value) {
		return { type: 'abs', value }
	},

	rel(value) {
		return { type: 'rel', value }
	},

	none: { type: 'none', value: 0 }
}

const Padding1 = {
	all(value) {
		return { left: value, top: value, right: value, bottom: value }
	},

	only(side, value) {
		return {
			left: side === 'left' ? value : Value.none,
			top: side === 'top' ? value : Value.none,
			right: side === 'right' ? value : Value.none,
			bottom: side === 'bottom' ? value : Value.none
		}
	},

	symmetrical(horiz, vert) {
		return { left: horiz, top: vert, right: horiz, bottom: vert }
	}
}

const FlexDirection = {
	row: 0,
	column: 1
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

const Label = ({ children }) => (
	<sized sizeConstraints={{ width: Value.abs(100), height: Value.abs(30) }}>
		<text text={children} />
	</sized>
)

const Button = ({ children, isActive }) => (
	<padding padding={Padding1.only('right', Value.abs(16))}>
		<sized
			sizeConstraints={{ width: Value.abs(125), height: Value.abs(30) }}
		>
			<stack>
				<background color={isActive ? COLOR_ACTIVE : Color.LIGHTGRAY} />
				<center>
					<text text={children} />
				</center>
			</stack>
		</sized>
	</padding>
)

const Row = ({ children }) => (
	<padding padding={Padding1.only('bottom', Value.abs(24))}>
		<flex>{children}</flex>
	</padding>
)

const App = () => {
	return (
		<stack>
			<background color={Color.WHITE} />
			<align align={{ left: Value.abs(20), top: Value.abs(20) }}>
				<flex direction={FlexDirection.column}>
					<sized
						sizeConstraints={{
							width: Value.abs(300),
							height: Value.abs(100)
						}}
					>
						<flex>
							<Box color={Color.RED} />
							<Box color={Color.GREEN} />
							<Box color={Color.BLUE} />
						</flex>
					</sized>
					<Row>
						<Label>Direction</Label>
						<Button isActive>row</Button>
						<Button>column</Button>
					</Row>
					<Row>
						<Label>Justify</Label>
						<intrinsicHeight>
							<flex direction={FlexDirection.column}>
								<padding
									padding={Padding1.only(
										'bottom',
										Value.abs(12)
									)}
								>
									<flex>
										<Button isActive>start</Button>
										<Button>center</Button>
										<Button>end</Button>
									</flex>
								</padding>
								<flex>
									<Button>spaceAround</Button>
									<Button>spaceBetween</Button>
									<Button>spaceEvenly</Button>
								</flex>
							</flex>
						</intrinsicHeight>
					</Row>
					<Row>
						<Label>Align</Label>
						<Button isActive>start</Button>
						<Button>center</Button>
						<Button>end</Button>
					</Row>
				</flex>
			</align>
		</stack>
	)
}

ReactAardvark.render(<App />, document)
