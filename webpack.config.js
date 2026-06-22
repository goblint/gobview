const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const webpack = require('webpack');

module.exports = {
    mode: process.env.NODE_ENV ? process.env.NODE_ENV : 'development',
    entry: './src/App.bc.js',
    module: {
        rules: [
            {
                test: /\.s?css$/,
                use: [
                    'style-loader',
                    'css-loader',
                    {
                        loader: 'postcss-loader',
                        options: { postcssOptions: { plugins: ['autoprefixer'] } },
                    },
                    'sass-loader',
                ],
            },
            {
                test: /\.ttf$/,
                use: 'file-loader',
            },
        ],
    },
    plugins: [
        new HtmlWebpackPlugin({ template: 'public/index.html' }),
        new webpack.IgnorePlugin({ resourceRegExp: /^node:/ }),
    ],
    resolve: { fallback: { child_process: false, constants: false, fs: false, tty: false, 'node:constants': false, 'node:child_process': false, 'node:fs': false, 'node:tty': false } },
    devServer: {
        contentBase: path.resolve(__dirname, 'public'),
    },
};
