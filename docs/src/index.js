import '@babel/polyfill'
import ReactDOM from 'react-dom'
import React from 'react'
import MiniBook from 'minibook'
import { dark } from 'minibook/lib/themes'
import 'minibook/lib/styles.css'

import DesignSection from './design'
import CppApiSection from './cpp-api'
import JsApiSection from './js-api'

const sections = {
	'design': DesignSection,
	'cpp-api': CppApiSection,
	'js-api': JsApiSection
}

ReactDOM.render(
	<MiniBook title="Aardvark" sections={sections} theme={dark} />,
	document.querySelector('#root')
);
