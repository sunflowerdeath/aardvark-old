let path = require('path')
let { capitalize } = require('../idl/node_modules/lodash')
let gen = require('../idl/index.js')

let items = [
    'enum', 'struct', 'class', 'extends', 'function', 'callback', 'proxy'
]

items.map(item => gen({
    src: path.resolve(__dirname, `../tests/idl/${item}.yaml`),
    output: {
        dir: path.resolve(__dirname, '../generated'),
        namespace: 'test',
        classname: `Test${capitalize(item)}Api`,
        filename: item
    }
}))
