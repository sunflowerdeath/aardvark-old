const path = require('path')

module.exports = {
    entry: './src/react.js',
    output: {
        path: path.resolve(__dirname),
        filename: 'src.js'
    },
    mode: 'development'
}