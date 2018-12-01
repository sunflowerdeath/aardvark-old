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
		translate: Position.origin,
		scale: 1,
		opacity: 1,
		rotation: 0
	})
}

export default ComposeOptions
