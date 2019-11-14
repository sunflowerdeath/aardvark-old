const path = require('path')

module.exports = {
    entry: './src/index.js',
    output: {
        path: path.resolve(__dirname, 'build'),
        filename: 'main.js'
    },
    mode: process.env.NODE_ENV == 'production' ? 'production' : 'development',
    module: {
        rules: [
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        plugins: [
                            '@babel/plugin-proposal-optional-chaining',
                            '@babel/plugin-proposal-function-bind',
                            '@babel/plugin-proposal-partial-application'
                        ],
                        presets: [
                            '@babel/preset-react',
                            [
                                '@babel/preset-env',
                                {
                                    targets: { safari: '12' },
                                    useBuiltIns: 'usage',
                                    modules: 'false',
                                    corejs: { version: 3 }
                                }
                            ]
                        ]
                    }
                }
            }
        ]
    },
    devtool: 'source-map'
}
