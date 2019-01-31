import Index from './index.md'
import Element from './element.md'
import SingleChildElement from './single-child-element.md'
import InlineLayoutLineBreak from './inline-layout-line-break.md'

export default {
	name: 'C++ API',
	stories: {
	    'index': { name: 'Index', markdown: Index },
		'element': { name: 'class Element', markdown: Element },
		'single-child-element': { name: 'class SingleChildElement', markdown: SingleChildElement },
		'inline-layout-line-break': {
            name: 'enum class inline_layout::LineBreak',
            markdown: InlineLayoutLineBreak
        }
	}
}
