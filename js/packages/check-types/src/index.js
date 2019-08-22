const check = {}

function CheckError(message) {
	this.message = message
	this.name = 'CheckError'
	// Do not capture stack for better performance
	this.stack = ''
}
// make `instanceof Error` work
CheckError.prototype = Object.create(Error.prototype)

const ANONYMOUS = '<<anonymous>>'

const nullish = value => value === null || value === undefined

const isSymbol = (value, type) => {
	// Native Symbol
	if (type === 'symbol') return true
	// Polyfilled Symbol
	if (typeof Symbol === 'function' && value instanceof Symbol) return true
	return false
}

const getType = value => {
	const type = typeof value
	if (Array.isArray(value)) return 'array'
	if (isSymbol(value, type)) return 'symbol'
	return type
}

const createPrimitive = expectedType => (value, name, location) => {
	const type = getType(value)
	if (type !== expectedType) {
		return new CheckError(
			`Invalid property \`${name}\` of type \`${type}\` supplied ` +
				`to ${location}, expected \`${expectedType}\`.`
		)
	}
}

const createOptional = checker => (value, name, location) => {
    if (nullish(value)) return
    const error = checker(value, name, location)
    if (error) return error
}

const createArrayOf = typeChecker => (value, name, location) => {
	const arrayError = check.array(value, name, location)
	if (arrayError) return arrayError

	for (let i = 0; i < value.length; i++) {
		const error = typeChecker(value[i], name + '[' + i + ']', location)
		if (error) return error
	}
}

const createObjectOf = typeChecker => (value, name, location) => {
	const objectError = check.object(value, name, location)
	if (objectError) return objectError

	for (const key in value) {
		if (value.hasOwnProperty(key)) {
			const error = typeChecker(value[key], name + '.' + key, location)
			if (error) return error
		}
	}
}

const stringifyValue = value => {
	if (isSymbol(value)) return String(value)
	return JSON.stringify(value)
}

const stringifyValues = values => values.map(stringifyValue).join(', ')

const createEnum = values => (value, name, location) => {
	if (values.indexOf(value) !== -1) return
	const valuesString = stringifyValues(values)
	return Error(
		`Invalid property \`${name}\` of value \`${value}\` supplied ` +
			`to ${location}, expected one of ${valuesString}.`
	)
}

const createUnion = checkers => (value, name, location) => {
	for (let i = 0; i < checkers.length; i++) {
		if (checkers[i](value, name, location) === undefined) return
	}
	return Error(
		`Invalid property \`${name}\` of value \`${value}\` supplied ` +
			`to ${location}.`
	)
}

const getClassName = value => {
	if (!value.constructor || !value.constructor.name) return ANONYMOUS
	return value.constructor.name
}

const createInstanceOf = expectedClass => (value, name, location) => {
	if (value instanceof expectedClass) return
	const actualClassName = getClassName(value)
	const expectedClassName = expectedClass.name || ANONYMOUS
	return new CheckError(
		`Invalid property \`${name}\` of type \`${actualClassName}\` ` +
			`supplied to ${location}, expected instance of ` +
			`\`${expectedClassName}\`.`
	)
}

const stringifyKeys = object =>
	Object.keys(object)
		.map(key => `\`${key}\``)
		.join(', ')

const createShape = ({ shape, loose }) => (value, name, location) => {
	const objectError = check.object(value, name, location)
	if (objectError) return objectError

	if (!loose) {
		for (let key in value) {
			if (!(key in shape)) {
				if (nullish(name)) {
					return new CheckError(
						`Invalid property \`${key}\` supplied to ${location}.` +
							` Valid properties are: ${stringifyKeys(shape)}.`
					)
				} else {
					return new CheckError(
						`Invalid key \`${key}\` supplied to property ` +
							`\`${name}\` of ${location}. Valid keys are: ` +
							`${stringifyKeys(shape)}.`
					)
				}
			}
		}
	}

	for (let key in shape) {
        const propName = nullish(name) ? key : `${name}.${key}`
		const error = shape[key](value[key], propName, location)
		if (error)  return error
	}
}

Object.assign(check, {
	array: createPrimitive('array'),
	bool: createPrimitive('boolean'),
	func: createPrimitive('function'),
	number: createPrimitive('number'),
	object: createPrimitive('object'),
	string: createPrimitive('string'),
	symbol: createPrimitive('symbol'),
    optional: createOptional,
	arrayOf: createArrayOf,
	objectOf: createObjectOf,
	instanceOf: createInstanceOf,
	enum: createEnum,
	union: createUnion,
	shape: shape => createShape({ shape, loose: false }),
	looseShape: shape => createShape({ shape, loose: true }),
    CheckError
})

module.exports = check
