const path = require('path')

module.exports = {
	entry: './src/ws.js',
	output: {
		path: path.resolve(__dirname, 'build'),
		filename: 'main.js'
	},
	mode: 'development',
	module: {
		rules: [
			{
				test: /\.js$/,
				exclude: /node_modules/,
				use: {
					loader: 'babel-loader',
					options: {
						presets: ['@babel/preset-react']
					}
				}
			}
		]
	},
    devtool: false
}
