import React, { useState, useCallback } from 'react'
import { Color, Value, Padding as Padding1, FlexDirection } from '@advk/common'
import ReactAardvark, {
	GestureResponder,
	Sized,
	Stack,
	Background,
	Center,
	Padding,
	IntrinsicWidth,
	IntrinsicHeight,
	Flex,
	Text
} from '@advk/react-renderer'

const win = application.createWindow(640, 480)
const document = application.getDocument(win)

const BTN_INITIAL_COLOR = { red: 238, green: 238, blue: 238, alpha: 255 }
const BTN_HOVERED_COLOR = { red: 165, green: 214, blue: 167, alpha: 255 }
const BTN_PRESSED_COLOR = { red: 255, green: 171, blue: 145, alpha: 255 }

const Button = ({ children, onTap }) => {
	const [isPressed, setIsPressed] = useState(false)
	const [isHovered, setIsHovered] = useState(false)
	return (
		<GestureResponder
			onTap={onTap}
			onPressStart={useCallback(() => setIsPressed(true))}
			onPressEnd={useCallback(() => setIsPressed(false))}
			onHoverStart={useCallback(() => setIsHovered(true))}
			onHoverEnd={useCallback(() => setIsHovered(false))}
		>
			<Sized sizeConstraints={{ height: Value.abs(40) }}>
				<Stack>
					<Background
						color={
							isPressed
								? BTN_PRESSED_COLOR
								: isHovered
								? BTN_HOVERED_COLOR
								: BTN_INITIAL_COLOR
						}
					/>
					<Center>
						<Padding padding={Padding1.horiz(16)}>
							<IntrinsicWidth>{children}</IntrinsicWidth>
						</Padding>
					</Center>
				</Stack>
			</Sized>
		</GestureResponder>
	)
}

const INITIAL_COLOR = { red: 179, green: 229, blue: 252, alpha: 255 }
const HOVERED_COLOR = { red: 79, green: 195, blue: 247, alpha: 255 }

const Panel = ({ children }) => {
	const [isHovered, setIsHovered] = useState(false)
	return (
		<GestureResponder
			onHoverStart={useCallback(() => setIsHovered(true))}
			onHoverEnd={useCallback(() => setIsHovered(false))}
		>
			<IntrinsicHeight>
				<Sized sizeConstraints={{ width: Value.abs(200) }}>
					<Stack>
						<Background
							color={isHovered ? HOVERED_COLOR : INITIAL_COLOR}
						/>
						<Padding padding={Padding1.all(16)}>
							<Flex direction={FlexDirection.column}>
								{children}
							</Flex>
						</Padding>
					</Stack>
				</Sized>
			</IntrinsicHeight>
		</GestureResponder>
	)
}

const App = () => (
	<Stack>
		<Background color={Color.WHITE} />
		<Padding padding={Padding1.all(16)}>
			<Panel>
				<Padding padding={Padding1.only('bottom', 16)}>
					<Button onTap={() => log('Tap first button')}>
						<Text text="Button 1" />
					</Button>
				</Padding>
				<Button onTap={() => log('Tap second button')}>
					<Text text="Button 2" />
				</Button>
			</Panel>
		</Padding>
	</Stack>
)

ReactAardvark.render(<App />, document)
