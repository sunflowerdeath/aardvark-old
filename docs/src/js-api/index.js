import Index from './index.md'
import React from './react.md'

import Align from './elements/align.md'
import Sized from './elements/sized.md'
import Elements from './elements/elements.md'

export default {
	name: 'JS API',
	stories: {
	    'index': { name: 'Index', markdown: Index },
	    'react-renderer': { name: 'React renderer', markdown: React },
        'align': { name: 'Align', markdown: Align },
        'sized': { name: 'Sized', markdown: Sized },
        'elements': { name: 'Elements', markdown: Elements }
	}
}
