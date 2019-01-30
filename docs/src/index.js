import '@babel/polyfill'
import ReactDOM from 'react-dom'
import React from 'react'
import MiniBook from 'minibook'
import 'minibook/lib/styles.css'

import CppApiSection from './cpp-api'

const sections = {
	'cpp-api': CppApiSection
}

ReactDOM.render(
	<MiniBook title="Aardvark" sections={sections} />,
	document.querySelector('#root')
);