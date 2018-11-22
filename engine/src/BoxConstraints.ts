import Size from './Size'

interface IBoxConstraints {
	minWidth: number
	maxWidth: number
	minHeight: number
	maxHeight: number
}

class BoxConstraints implements IBoxConstraints{
	constructor(data: IBoxConstraints) {
		Object.assign(this, data)
	}

	minWidth: number
	maxWidth: number
	minHeight: number
	maxHeight: number

	makeLoose(): BoxConstraints {
		return new BoxConstraints({
			minWidth: 0,
			maxWidth: this.maxWidth,
			minHeight: 0,
			maxHeight: this.maxHeight
		})
	}

	static isTight(constraints: BoxConstraints) {
		return (
			constraints.minWidth === constraints.maxWidth &&
			constraints.minHeight === constraints.maxHeight
		)
	}

	static fromSize(size: Size) {
		return new BoxConstraints({
			minWidth: size.width,
			maxWidth: size.width,
			minHeight: size.height,
			maxHeight: size.height
		})
	}
}

export default BoxConstraints
