import React from 'react'
import { Value, Color } from '@advk/common'
import ReactAardvark, {
    Align,
    Size,
    Stack,
    Background,
    Paragraph
} from '@advk/react-renderer'

const ParagraphExample = () => {
    let text = "Of course, most of us are not going to have that opportunity."
    return (
        <Align insets={{ left: Values.abs(10) }}>
            <Size sizeConstraints={{ width: Value.abs(200) }}>
                <Paragraph>
                    <TextSpan text={text} />
                </Paragraph>
            </Size>
        </Align>
    )
}

export default ParagraphExample
