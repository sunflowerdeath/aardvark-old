import ElementStory from './element.md'
import InlineLayoutLineBreak from './inline-layout-line-break.md'

export default {
	name: 'C++ API',
	stories: {
		'element': { name: 'class Element', markdown: ElementStory },
		'inline-layout-line-break': {
            name: 'enum class inline_layout::LineBreak',
            markdown: InlineLayoutLineBreak
        }
	}
}
