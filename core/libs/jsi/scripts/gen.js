let path = require('path')
let { capitalize } = require('../idl/node_modules/lodash')
let gen = require('../idl/index.js')

let items = [
    'enum', 'union', 'struct', 'class', 'extends', 'function', 'callback',
    'proxy', 'custom', 'optional'
]

items.map(item => gen({
    src: path.resolve(__dirname, `../tests/idl/${item}.yaml`),
    output: {
        dir: path.resolve(__dirname, '../generated'),
        namespace: 'test',
        class: `Test${capitalize(item)}Api`,
        filename: item
    }
}))
