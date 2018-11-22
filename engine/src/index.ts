import Compositor from './Compositor'
import Document from './Document'
import Size from './Size'
import { Center, FixedSize, Background, Stack, Align }  from './elements/index'

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
				size: new Size({ width: 50, height: 50 }),
				child: new Background({ color: 'green' })
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
