import React from 'react'
import { Value, Color, BoxBorders, BorderSide } from '@advk/common'
import ReactAardvark, {
    Align,
    Size,
    Stack,
    Background,
    Paragraph,
    TextSpanC,
    DecorationSpanC,
    IntrinsicHeight,
    Text
} from '@advk/react-renderer'

const ParagraphExample = () => {
    let text = 'Of course, most of us are not going to have that opportunity.'
    return (
        <Align insets={{ left: Value.abs(10), top: Value.abs(0) }}>
            <Size sizeConstraints={{ width: Value.abs(200) }}>
                <Paragraph>
                    <DecorationSpanC
                        decoration={{
                            background: Color.lightgrey
                        }}
                    >
                        <TextSpanC text={text} />
                        <DecorationSpanC
                            decoration={{
                                borders: BoxBorders.all(
                                    BorderSide(2, Color.red)
                                )
                            }}
                        >
                            <TextSpanC text="MORE TEXT" />
                        </DecorationSpanC>
                    </DecorationSpanC>
                </Paragraph>
            </Size>
        </Align>
    )
}

export default ParagraphExample
