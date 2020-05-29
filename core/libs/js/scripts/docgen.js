let path = require('path')
let docgen = require('../../jsi/idl/docgen')

let src = [
    'animation_frame',
    'base_types',
    'events',
    'desktop_app',
    'desktop_window',
    'document',
    'element',
    'elements',
    'inline'
]

docgen({
    src: src.map(file => path.resolve(__dirname, `../api/${file}.yaml`)),
    output: {
        dir: path.resolve(__dirname, '../../../../docs/generated')
    }
})
