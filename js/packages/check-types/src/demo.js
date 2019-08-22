const chalk = require('chalk')
const check = require('./index.js')

const logTitle = title => {
	console.log(chalk.bold.cyan(title))
}

const logError = (checker, value, name = 'propName') => {
	console.log('* ' + checker(value, name, 'Location').message)
}

logTitle('number')
logError(check.number, 'a')
console.log('\n')

logTitle('arrayOf')
logError(check.arrayOf(check.number), 'a')
logError(check.arrayOf(check.number), [1, 'a'])
console.log('\n')

logTitle('objectOf')
logError(check.objectOf(check.number), [1, 2, 3])
logError(check.objectOf(check.number), { a: 1, b: 'b' })
console.log('\n')

class A {}
class B {}
logTitle('instanceOf')
logError(check.instanceOf(A), 0)
logError(check.instanceOf(A), {})
logError(check.instanceOf(A), () => {
	console.log('test')
})
logError(check.instanceOf(A), new B())
console.log('\n')

logTitle('enum')
logError(check.enum(['1', '2', '3']), 0)
logError(check.enum(['1', '2', '3']), {})
console.log('\n')

logTitle('union')
logError(check.union([check.bool, check.string]), 0)
logError(check.union([check.bool, check.string]), {})
logError(check.union([check.bool, check.string]), () => {
	console.log('test')
})
console.log('\n')

const shape = { a: check.number, b: check.string }

logTitle('looseShape')
logError(check.looseShape(shape), 0)
logError(check.looseShape(shape), { a: 1, b: 2 })
logError(check.looseShape(shape), { a: 1 })
console.log('\n')

logTitle('shape')
logError(check.shape(shape), 0)
logError(check.shape(shape), { a: 1, b: 1 })
console.log('nested')
logError(check.shape(shape), { a: 1 })
logError(check.shape(shape), { a: 1, b: 'b', c: 3 })
console.log('global')
logError(check.shape(shape), { a: 1 }, null)
logError(check.shape(shape), { a: 1, b: 'b', c: 3 }, null)
console.log('\n')
