interface IPosition {
	top: number
	left: number
}

class Position {
	constructor(data: IPosition) {
		Object.assign(this, data)
	}

	top: number
	left: number

	static origin = new Position({ top: 0, left: 0 })

	static add(a: Position, b: Position) {
		return new Position({
			top: a.top + b.top,
			left: a.left + b.left
		})
	}
}

export default Position
