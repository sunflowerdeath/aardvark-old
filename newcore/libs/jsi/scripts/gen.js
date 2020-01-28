let path = require('path')
let { capitalize } = require('../idl/node_modules/lodash')
let gen = require('../idl/index.js')

let items = ['enum', 'struct', 'class']

items.map(item => gen({
    src: path.resolve(__dirname, `../tests/idl/${item}.yaml`),
    namespace: 'test',
    classname: `Test${capitalize(item)}Api`,
    outputDir: path.resolve(__dirname, '../generated'),
    filename: item
}))
