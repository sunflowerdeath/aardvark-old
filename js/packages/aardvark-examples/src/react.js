import React from 'react'
import ReactDOMServer from 'react-dom/server'

let Hello = () => React.createElement('div', null, 'Hello world!')

let res = ReactDOMServer.renderToString(React.createElement(Hello))

log(res)