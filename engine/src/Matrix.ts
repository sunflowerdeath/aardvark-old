class Matrix {
	a: number
	b: number
	c: number
	d: number
	tx: number
	ty: number

	constructor() {
		Object.assign(this, { a: 1, b: 0, c: 0, d: 1, tx: 0, ty: 0 })
	}

	translate(x, y) {
		this.tx += x * this.a + y * this.c
		this.ty += x * this.b + y * this.d
		return this
	}

	scale(x, y) {
		this.a *= x
		this.b *= x
		this.c *= y
		this.d *= y
		return this
	}

	rotate(angle, x, y) {
		if (x !== undefined) this.translate(x, y)
		const cos = Math.cos(angle)
		const sin = Math.sin(angle)
		const { a, b, c, d } = this
		this.a = cos * a + sin * c
		this.b = cos * b + sin * d
		this.c = -sin * a + cos * c
		this.d = -sin * b + cos * d
		if (x !== undefined) this.translate(-x, -y)
		return this
	}

	toArray() {
		const { a, b, c, d, tx, ty } = this
		return [a, b, c, d, tx, ty]
	}

	/*
	prepend(mx) {
		const { a: a1, b: b1, c: c1, d: d1, tx: tx1, ty: ty1 } = this
		const { a: a2, b: b2, c: c2, d: d2, tx: tx2, ty: ty2 } = mx
		this.a = a1 * a2 + b1 * c2
		this.b = a1 * b2 + b1 * d2
		this.c = c1 * a2 + d1 * c2
		this.d = c1 * b2 + d1 * d2
		this.tx = a2 * tx1 + c2 * ty1 + tx2
		this.ty = b2 * tx1 + d2 * ty1 + ty2
		return this
	}
	*/

	calc(x: number, y: number): [number, number] {
		const {a, b, c, d, tx, ty} = this
		return [a * x + c * y + tx, b * x + d * y + ty]
	}
}

export default Matrix
