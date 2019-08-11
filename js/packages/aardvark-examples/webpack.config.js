const path = require('path')

module.exports = {
	entry: './src/timeout.js',
	output: {
		path: path.resolve(__dirname),
		filename: 'src.js'
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
	}
}
