const path = require('path')

module.exports = {
	entry: './src/getOriginalLocation.js',
	output: {
		path: path.resolve(__dirname, 'build'),
		filename: 'getOriginalLocation.js',
		library: 'getOriginalLocation',
		libraryTarget: 'this'
	},
	mode: 'development',
    devtool: false
}
