import Index from './index.md'

import Element from '../../generated/Element.md'
import AlignedElement from '../../generated/AlignedElement.md'
import BackgroundElement from '../../generated/BackgroundElement.md'
import BorderElement from '../../generated/BorderElement.md'

export default {
	name: 'JS API',
	stories: {
	    'index': { name: 'Index', markdown: Index },
	    'element': { name: 'Element', markdown: Element },
	    'aligned-element': { name: 'AlignedElement', markdown: AlignedElement },
	    'background-element': { name: 'BackgroundElement', markdown: BackgroundElement },
	    'border-element': { name: 'BorderElement', markdown: BorderElement }
	}
}
