import { registerNativeComponent as register } from './helpers.js'

export const Aligned = register('Aligned', AlignedElement)
export const Background = register('Background', BackgroundElement)
export const Border = register('Border', BorderElement)
export const Center = register('Center', CenterElement)
export const Clip = register('Clip', ClipElement)
export const CustomLayout = register('CustomLayout', CustomLayoutElement)
export const IntrinsicHeight = register(
    'IntrinsicHeight',
    IntrinsicHeightElement
)
export const IntrinsicWidth = register('IntrinsicWidth', IntrinsicWidthElement)
export const Image = register('Image', ImageElement)
export const Flex = register('Flex', FlexElement)
export const FlexChild = register('FlexChild', FlexChildElement)
export const Layer = register('Layer', LayerElement)
export const Padded = register('Padded', PaddedElement)
export const Responder = register('Responder', ResponderElement)
export const Sized = register('Sized', SizedElement)
export const Stack = register('Stack', StackElement)
export const StackChild = register('StackChild', StackChildElement)
export const Text = register('Text', TextElement)
export const Translated = register('Translated', TranslatedElement)
export const Paragraph = register('Paragraph', ParagraphElement)
export const Placeholder = register('Placeholder', PlaceholderElement)
export const TextSpanC = register('TextSpan', TextSpan)
export const DecorationSpanC = register('DecorationSpan', DecorationSpan)
export const ResponderSpanC = register('ResponderSpan', ResponderSpan)
