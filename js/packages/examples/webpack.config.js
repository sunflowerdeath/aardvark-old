const path = require('path')

module.exports = {
    entry: {
        main: './src/index.js',
        todomvc: './src/todomvc/index.js'
    },
    output: {
        path: path.resolve(
            __dirname,
            process.env.NODE_ENV == 'production' ? 'build-prod' : 'build'
        ),
        filename: '[name].js'
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
    devtool: process.env.NODE_ENV == 'production' ? false : 'source-map'
}
