import React, { useMemo } from 'react'
import { CustomLayout } from '../nativeComponents'

const layout = (elem, constraints) => {
	const scrollHeight = 0
	for (const child of elem.children) {
		const childConstraints = {
			minWidth: 0,
			maxWidth: constraints.maxWidth,
			minHeight: 0,
			maxHeight: Infinity
		}
		const childSize = elem.document.layoutElement(child, childConstraints)
		child.setLayoutProps({ left: 0, top: scrollHeight }, childSize)
		scrollHeight += childSize.height
	}
	return { width: constraints.maxWidth, height: scrollHeight }
}

const Scrolled = props => {
	const { children, scrollTop } = props
	const transform = useMemo(
		() => new TransformMatrix().translate({ left: 0, top: -scrollTop }),
		[scrollTop]
	)
	return (
		<Clip>
			<Layer transform={transform}>
				<CustomLayout layout={layout}>{children}</CustomLayout>
			</Layer>
		</Clip>
	)
}

export default Scrolled
