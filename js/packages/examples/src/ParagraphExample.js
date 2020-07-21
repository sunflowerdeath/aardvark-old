import React, { useState } from 'react'
import { Alignment, Value, Color, BoxBorders, BorderSide } from '@advk/common'
import ReactAardvark, {
    Aligned,
    Sized,
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
    let p1 = (
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
                                    BorderSide(8, Color.yellow)
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
    )

    let p2 = (
        <Paragraph>
            <TextSpanC
                text="Underline underline"
                style={{
                    decorations: [{ kind: TextDecorationKind.underline }]
                }}
            />
            <TextSpanC
                text="Big underline"
                style={{
                    decorations: [
                        {
                            kind: TextDecorationKind.underline,
                            thickness: 4,
                            color: Color.red
                        }
                    ]
                }}
            />
            <TextSpanC
                text="Overline overline"
                style={{
                    decorations: [{ kind: TextDecorationKind.overline }]
                }}
            />
            <TextSpanC
                text="Big overline"
                style={{
                    decorations: [
                        {
                            kind: TextDecorationKind.overline,
                            thickness: 4,
                            color: Color.red
                        }
                    ]
                }}
            />
            <TextSpanC
                text="Linethrough linethrough"
                style={{
                    decorations: [{ kind: TextDecorationKind.lineThrough }]
                }}
            />
            <TextSpanC
                text="Big linethrough"
                style={{
                    decorations: [
                        {
                            kind: TextDecorationKind.lineThrough,
                            thickness: 3,
                            color: Color.red
                        }
                    ]
                }}
            />
        </Paragraph>
    )

    return (
        <Stack>
            <Aligned alignment={Alignment.topLeft(Value.abs(10), Value.abs(20))}>
                <Sized sizeConstraints={{ width: Value.abs(200) }}>{p1}</Sized>
            </Aligned>
            <Aligned
                alignment={Alignment.topLeft(Value.abs(200), Value.abs(20))}
            >
                <Sized sizeConstraints={{ width: Value.abs(200) }}>{p2}</Sized>
            </Aligned>
        </Stack>
    )
}

export default ParagraphExample
