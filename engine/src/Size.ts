class Size {
	constructor(data: Object) {
		Object.assign(this, data)
	}

	width: number
	height: number

	static isEqual(a: Size, b: Size) {
		return a.width === b.width && a.height === b.height
	}
}

export default Size
