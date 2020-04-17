import React from 'react'
import { Value, Color } from '@advk/common'
import ReactAardvark, {
    Align,
    Size,
    Stack,
    Background,
    Paragraph,
    TextSpanC,
    IntrinsicHeight,
    Text
} from '@advk/react-renderer'

const ParagraphExample = () => {
    let text = 'Of course, most of us are not going to have that opportunity.'
    return (
        <Align insets={{ left: Value.abs(10), top: Value.abs(0) }}>
            <Size sizeConstraints={{ width: Value.abs(200) }}>
                <IntrinsicHeight>
                    <Stack>
                        <Background color={Color.lightgrey} />
                        <Paragraph>
                            <TextSpanC text={text} />
                        </Paragraph>
                    </Stack>
                </IntrinsicHeight>
            </Size>
        </Align>
    )
}

export default ParagraphExample
