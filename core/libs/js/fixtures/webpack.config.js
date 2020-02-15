const path = require('path')

const inline = {
	entry: './src/index.js',
	output: {
		path: path.resolve(__dirname, 'build'),
		filename: 'inline-source-map.js'
	},
	mode: 'development',
    devtool: 'inline-source-map'
}

const external = {
	entry: './src/index.js',
	output: {
		path: path.resolve(__dirname, 'build'),
		filename: 'external-source-map.js'
	},
	mode: 'development',
    devtool: 'source-map'
}

module.exports = [inline, external]
