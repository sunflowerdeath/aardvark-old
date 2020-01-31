import '@babel/polyfill'
import ReactDOM from 'react-dom'
import React from 'react'
import MiniBook from 'minibook'
import 'minibook/lib/styles.css'
import './fonts/FantasqueSansMono.css'

import DesignSection from './design'
import CppApiSection from './cpp-api'
import JsApiSection from './js-api'

const sections = {
	'design': DesignSection,
	'cpp-api': CppApiSection,
	'js-api': JsApiSection
}

ReactDOM.render(
	<MiniBook title="Aardvark" sections={sections} />,
	document.querySelector('#root')
);
