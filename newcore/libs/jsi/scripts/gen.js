let path = require('path')
let gen = require('../idl/index.js')

gen({
    src: path.resolve(__dirname, '../tests/idl/enum.yaml'),
    outputDir: path.resolve(__dirname, '../generated'),
    filename: 'enum',
    namespace: 'test',
    classname: 'EnumTestApi'
})
