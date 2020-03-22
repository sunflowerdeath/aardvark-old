import { Color } from './Color.js'

const BorderSide = (width, color) => ({ width, color })

BorderSide.none = BorderSide(0, Color.black)

const BoxBorders = {
	all(side) {
		return { left: side, top: side, right: side, bottom: side }
	},

	only(side, value) {
		return {
			left: side === 'left' ? value : BorderSide.none,
			top: side === 'top' ? value : BorderSide.none,
			right: side === 'right' ? value : BorderSide.none,
			bottom: side === 'bottom' ? value : BorderSide.none
		}
	}
}

export { BorderSide, BoxBorders }
