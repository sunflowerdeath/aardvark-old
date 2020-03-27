const Padding = {
	all(value) {
		return { left: value, top: value, right: value, bottom: value }
	},

	only(side, value) {
		return {
			left: side === 'left' ? value : 0,
			top: side === 'top' ? value : 0,
			right: side === 'right' ? value : 0,
			bottom: side === 'bottom' ? value : 0
		}
	},

	symmetrical(horiz, vert) {
		return { left: horiz, top: vert, right: horiz, bottom: vert }
	},

	horiz(value) {
		return { left: value, top: 0, right: value, bottom: 0 }
	},

	vert(value) {
		return { left: 0, top: value, right: 0, bottom: value }
	}
}

Padding.none = Padding.all(0)

export { Padding }
