import React, { useState } from 'react'
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
import GestureResponderSpan from '@advk/react-renderer/src/components/GestureResponderSpan'

const ParagraphExample = () => {
    const [hovered, setHovered] = useState(false)
    let text = 'Of course, most of us are not going to have that opportunity.'
    return (
        <Align insets={{ left: Value.abs(10), top: Value.abs(0) }}>
            <Size sizeConstraints={{ width: Value.abs(200) }}>
                <IntrinsicHeight>
                    <Stack>
                        <Background color={Color.blue} />
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
                                <GestureResponderSpan
                                    onHoverStart={() => setHovered(true)}
                                    onHoverEnd={() => setHovered(false)}
                                >
                                    <DecorationSpanC
                                        decoration={{
                                            background: hovered
                                                ? Color.yellow
                                                : Color.red
                                        }}
                                    >
                                        <TextSpanC text="link text link text link text" />
                                    </DecorationSpanC>
                                </GestureResponderSpan>
                            </DecorationSpanC>
                        </Paragraph>
                    </Stack>
                </IntrinsicHeight>
            </Size>
        </Align>
    )
}

export default ParagraphExample
