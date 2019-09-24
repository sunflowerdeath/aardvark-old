export const Value = {
	abs(value) {
		return { type: 'abs', value }
	},

	rel(value) {
		return { type: 'rel', value }
	},

	none: { type: 'none', value: 0 }
}

