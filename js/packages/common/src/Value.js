export const ValueType = {
    none: 0,
    abs: 1,
    rel: 2
}

export const Value = {
	abs(value) {
		return { type: ValueType.abs, value }
	},

	rel(value) {
		return { type: ValueType.rel, value }
	},

	none: { type: ValueType.none, value: 0 }
}

