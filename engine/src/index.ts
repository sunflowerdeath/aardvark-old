import Compositor from './Compositor'
import Document from './Document'
import Size from './Size'
import { Center, FixedSize, Background, Stack, Align } from './elements/index'
import Border, {
	BoxBorder,
	BoxRadius,
	BorderSide,
	Radius
} from './elements/Border'

declare var global: any

const canvas = document.getElementById('canvas')
const compositor = canvas instanceof HTMLCanvasElement && new Compositor(canvas)

const background = new Background({ color: 'blue' })
const fixedSize = new FixedSize({
	size: new Size({ width: 100, height: 50 }),
	child: background
})
const center = new Center({ child: fixedSize }, { isRepaintBoundary: true })

const root = new Stack({
	children: [
		new FixedSize({
			size: new Size({ width: 100, height: 100 }),
			child: new Background({ color: 'lightblue' })
		}),
		center,
		new Align({
			left: Align.px(20),
			top: Align.px(80),
			child: new FixedSize({
				size: new Size({ width: 100, height: 100 }),
				child: new Border({
					border: BoxBorder.all(
						new BorderSide({ width: 3, color: 'red' })
					),
					radius: BoxRadius.all(Radius.circular(20)),
					child: new Background({ color: 'black' })
				})
			})
		}),

		new Align({
			left: Align.px(150),
			top: Align.px(80),
			child: new FixedSize({
				size: new Size({ width: 100, height: 100 }),
				child: new Border({
					border: new BoxBorder({
						top: new BorderSide({ width: 8, color: 'red' }),
						right: new BorderSide({ width: 16, color: 'blue' }),
						bottom: new BorderSide({ width: 0, color: 'red' }),
						left: new BorderSide({ width: 8, color: 'black' })
					}),
					radius: BoxRadius.all(Radius.zero),
					child: new Background({ color: '#ccc' })
				})
			})
		}),

		new Align({
			left: Align.px(300),
			top: Align.px(80),
			child: new FixedSize({
				size: new Size({ width: 100, height: 100 }),
				child: new Border({
					border: BoxBorder.all(
						new BorderSide({ width: 30, color: 'blue' })
					),
					radius: new BoxRadius({
						topLeft: Radius.zero,
						topRight: Radius.circular(10),
						bottomLeft: Radius.circular(30),
						bottomRight: Radius.circular(50)
					}),
					child: new Background({ color: 'black' })
				})
			})
		})
	]
})
const doc = new Document({ compositor, root })
doc.paint()

global.doc = doc

global.changeColor = () => {
	background.setProps({ color: 'red' })
	doc.paint()
}

global.changeSize = () => {
	fixedSize.setProps({ size: new Size({ width: 100, height: 100 }) })
	doc.paint()
}
