let path = require('path')
let idl = require('../../jsi/idl')

let src = [
    'animation_frame',
    'base_types',
    'events',
    'document',
    'element',
    'elements',
    'inline'
]

let desktop_src = [
    'desktop_app',
    'desktop_window'
]

let android_src = [
    'android_app'
]

idl({
    src: [...src, ...desktop_src].map(
        file => path.resolve(__dirname, `../api/${file}.yaml`)),
    defaultNamespace: 'aardvark',
    include: [
        '../include/aardvark_js/api/animation_frame.hpp',
        '../include/aardvark_js/api/element.hpp',
        '../include/aardvark_js/api/transform.hpp'
    ],
    output: {
        dir: path.resolve(__dirname, '../generated'),
        filename: 'api',
        // TODO until core moved to nested namespace `aardvark::ui` or
        // `aardvark::core` to prevent name conflicts between aardvark::Value
        // and aardvark::jsi::Value
        namespace: 'aardvark_js_api',
        class: `Api`
    }
})

idl({
    src: [...src, ...android_src].map(
        file => path.resolve(__dirname, `../api/${file}.yaml`)),
    defaultNamespace: 'aardvark',
    include: [
        '../include/aardvark_js/api/animation_frame.hpp',
        '../include/aardvark_js/api/element.hpp',
        '../include/aardvark_js/api/transform.hpp'
    ],
    output: {
        dir: path.resolve(__dirname, '../generated'),
        filename: 'android_api',
        namespace: 'aardvark_js_api',
        class: `AndroidApi`
    }
})

idl({
    src: path.resolve(__dirname, `../api/error_location.yaml`),
    defaultNamespace: 'aardvark::jsi',
    output: {
        dir: path.resolve(__dirname, '../generated'),
        filename: 'error_location_api',
        namespace: 'aardvark_js_api',
        class: `ErrorLocationApi`
    }
})
