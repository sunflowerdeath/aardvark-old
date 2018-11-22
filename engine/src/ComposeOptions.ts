import Position from './Position'

interface IComposeOptions {
	translate: Position
	scale: number
	opacity: number
	rotation: number
}

class ComposeOptions {
	constructor(data: IComposeOptions) {
		Object.assign(this, data)
	}

	translate: Position
	scale: number
	opacity: number
	rotation: number

	static defaultValue = new ComposeOptions({
		translate: new Position({ top: 0, left: 0 }),
		scale: 1,
		opacity: 1,
		rotation: 0
	})
}

export default ComposeOptions
