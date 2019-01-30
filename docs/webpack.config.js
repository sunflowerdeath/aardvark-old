const webpack = require('webpack')
const baseConfig = require('gnoll/config/webpack')
const babelConfig = require('gnoll/config/babel')
const stylesConfig = require('gnoll-styles')
const merge = require('webpack-merge')

const PRODUCTION = process.env.NODE_ENV === 'production'

module.exports = merge(baseConfig, stylesConfig, {
	output: {
		// publicPath: PRODUCTION ? 'https://sunflowerdeath.github.io/' : '/'
	},
	module: {
		rules: [
			{
				test: /\.md$/,
				use: [
					{ loader: 'babel-loader', options: babelConfig },
					{
						loader: 'minimark-loader',
						options: require('minibook/minimark-preset')
					}
				]
			}
		]
	},
	devServer: {
		port: 1337,
		host: '0.0.0.0'
	},
	resolve: {
		modules: [
			'node_modules'
		]
	},
	resolveLoader: {
		modules: [
			'node_modules'
		]
	},
})
