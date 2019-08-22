const assert = require('assert')
const check = require('./index.js')

describe('check', () => {
	const expectValid = (checker, value) => {
		const res = checker(value)
		assert(
			res === undefined,
			`Expected value ${JSON.stringify(value)} to be valid\n` +
				`Error: ${res && res.message}`
		)
	}

	const expectError = (checker, value) => {
		const res = checker(value)
		assert(
			res instanceof Error,
			`Expected value ${JSON.stringify(value)} to give error`
		)
	}

	it('array', () => {
		expectValid(check.array, [])
		expectError(check.array, 'a')
	})

	it('bool', () => {
		expectValid(check.bool, true)
		expectError(check.bool, 'a')
	})

	it('func', () => {
		expectValid(check.func, () => {})
		expectError(check.func, 'a')
	})

	it('number', () => {
		expectValid(check.number, 1)
		expectError(check.number, 'a')
	})

	it('optional', () => {
		const checker = check.optional(check.number)
		expectValid(checker, 1)
		expectValid(checker, null)
		expectValid(checker, undefined)
		expectError(checker, 'a')
		expectError(checker, false)
	})

	it('arrayOf', () => {
		const checker = check.arrayOf(check.number)
		expectValid(checker, [])
		expectValid(checker, [1, 2, 3])
		expectError(checker, 'a')
		expectError(checker, [1, 2, 'a'])
	})

	it('objectOf', () => {
		const checker = check.objectOf(check.number)
		expectValid(checker, {})
		expectValid(checker, { a: 1, b: 2 })
		expectError(checker, 'a')
		expectError(checker, { a: 'a', b: 'b' })
	})

	it('instanceOf', () => {
		class A {}
		class B {}
		const checker = check.instanceOf(A)
		expectValid(checker, new A())
		expectError(checker, new B())
		expectError(checker, {})
	})

	it('enum', () => {
		const checker = check.enum(['a', 1])
		expectValid(checker, 1)
		expectValid(checker, 'a')
		expectError(checker, 2)
	})

	it('union', () => {
		const checker = check.union([check.number, check.bool])
		expectValid(checker, 1)
		expectValid(checker, true)
		expectError(checker, 'a')
	})

	it('shape', () => {
		const checker = check.shape({ a: check.number, b: check.string })
		expectValid(checker, { a: 1, b: 'b' })
		expectError(checker, { a: 1, b: 'b', c: 'c' })
		expectError(checker, 1)
		expectError(checker, { a: 'a' })

		const optionalChecker = check.shape({ a: check.optional(check.number) })
		expectValid(optionalChecker, {})
		expectValid(optionalChecker, { a: undefined })
		expectError(optionalChecker, { a: 'a' })
	})

	it('looseShape', () => {
		const checker = check.looseShape({ a: check.number, b: check.string })
		expectValid(checker, { a: 1, b: 'b' })
		expectValid(checker, { a: 1, b: 'b', c: 'c' })
		expectError(checker, 1)
		expectError(checker, { a: 'a' })
	})
})
