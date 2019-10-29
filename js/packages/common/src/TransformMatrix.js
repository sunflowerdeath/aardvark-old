const rowxcol = (a, b, row, col) =>
	a[row * 3] * b[col] +
	a[row * 3 + 1] * b[3 + col] +
	a[row * 3 + 2] * b[6 + col]

const TransformMatrix = {
	// prettier-ignore
	identity: [
        1, 0, 0,
        0, 1, 0,
        0, 0, 1],

	makeTranslate(dx, dy) {
		// prettier-ignore
		return [
            1, 0, dx,
            0, 1, dy,
            0, 0, 1]
	},

	makeScale(sx, sy) {
		// prettier-ignore
		return new Matrix(
            sx, 0,  0,
            0,  sy, 0,
            0,  0,  1)
	},

	makeRotate(angle, px, py) {
		const cos = Math.cos(angle)
		const sin = Math.sin(angle)
		// prettier-ignore
		return [
            cos, -sin, (sin * py + (1-cos) * px),
            sin, cos, (-sin * px + (1-cos) * py),
            0, 0, 1]
	},

	append(a, b) {
		// prettier-ignore
		return [
            rowxcol(a, b, 0, 0), rowxcol(a, b, 0, 1), rowxcol(a, b, 0, 2),
            rowxcol(a, b, 1, 0), rowxcol(a, b, 1, 1), rowxcol(a, b, 1, 2),
            rowxcol(a, b, 2, 0), rowxcol(a, b, 2, 1), rowxcol(a, b, 2, 2)]
	}
}

export { TransformMatrix }
