import React from 'react'

import Index from './index.md'

import Document from '../../generated/Document.md'
import Element from '../../generated/Element.md'

import DesktopWindow from '../../generated/DesktopWindow.md'
import DesktopApp from '../../generated/DesktopApp.md'

import Size from '../../generated/Size.md'
import Position from '../../generated/Position.md'
import ValueType from '../../generated/ValueType.md'
import Value from '../../generated/Value.md'
import Color from '../../generated/Color.md'
import Connection from '../../generated/Connection.md'

import PointerEvent from '../../generated/PointerEvent.md'
import PointerAction from '../../generated/PointerAction.md'
import PointerTool from '../../generated/PointerTool.md'
import ScrollEvent from '../../generated/ScrollEvent.md'
import KeyAction from '../../generated/KeyAction.md'
import KeyEvent from '../../generated/KeyEvent.md'

import Alignment from '../../generated/Alignment.md'
import AlignmentOrigin from '../../generated/AlignmentOrigin.md'
import AlignedElement from '../../generated/AlignedElement.md'

import BackgroundElement from '../../generated/BackgroundElement.md'

import Radius from '../../generated/Radius.md'
import BoxRadiuses from '../../generated/BoxRadiuses.md'
import BorderSide from '../../generated/BorderSide.md'
import BoxBorders from '../../generated/BoxBorders.md'
import BorderElement from '../../generated/BorderElement.md'

import CenterElement from '../../generated/CenterElement.md'
import ClipElement from '../../generated/ClipElement.md'
import CustomLayoutElement from '../../generated/CustomLayoutElement.md'

import FlexElement from '../../generated/FlexElement.md'
import FlexChildElement from '../../generated/FlexChildElement.md'
import FlexDirection from '../../generated/FlexDirection.md'
import FlexAlign from '../../generated/FlexAlign.md'
import FlexJustify from '../../generated/FlexJustify.md'

import IntrinsicHeightElement from '../../generated/IntrinsicHeightElement.md'
import IntrinsicWidthElement from '../../generated/IntrinsicWidthElement.md'

import LayerElement from '../../generated/LayerElement.md'
import TransformMatrix from './TransformMatrix.md'

const BaseTypes = () => <>
    <Size/>
    <Position/>
    <Color/>
    <ValueType/>
    <Value/>
    <Connection/>
</>

const Events = () => <>
    <PointerEvent/>
    <PointerAction/>
    <PointerTool/>
    <ScrollEvent/>
    <KeyAction/>
    <KeyEvent/>
</>

const Desktop = () => <>
    <DesktopApp/>
    <DesktopWindow/>
</>

const Aligned = () => <>
    <AlignedElement/>
    <AlignmentOrigin/>
    <Alignment/>
</>

const Border = () => <>
    <BorderElement/>
    <BorderSide/>
    <BoxBorders/>
    <Radius/>
    <BoxRadiuses/>
</>

const Flex = () => <>
    <FlexElement/>
    <FlexChildElement/>
    <FlexDirection/>
    <FlexJustify/>
    <FlexAlign/>
</>

const Intrinsic = () => <>
    <IntrinsicHeightElement/>
    <IntrinsicWidthElement/>
</>

const Layer = () => <>
    <LayerElement/>
    <TransformMatrix/>
</>

export default {
	name: 'JS API',
	stories: {
	    'index': { name: 'Index', markdown: Index },
	    'base-types': { name: 'Base types', markdown: BaseTypes },
	    'events': { name: 'Events', markdown: Events },
	    'desktop': { name: 'Desktop', markdown: Desktop },
	    'document': { name: 'Document', markdown: Document },
	    'element': { name: 'Element', markdown: Element },
	    'aligned-element': { name: 'AlignedElement', markdown: Aligned },
	    'background-element': { name: 'BackgroundElement', markdown: BackgroundElement },
	    'border-element': { name: 'BorderElement', markdown: Border },
	    'center-element': { name: 'CenterElement', markdown: CenterElement },
	    'clip-element': { name: 'ClipElement', markdown: ClipElement },
	    'custom-layout-element': { name: 'CustomLayoutElement', markdown: CustomLayoutElement },
	    'flex-element': { name: 'FlexElement', markdown: Flex },
	    'intrinsic-height': { name: 'IntrinsicHeightElement', markdown: IntrinsicHeightElement },
	    'intrinsic-width': { name: 'IntrinsicWidthElement', markdown: IntrinsicWidthElement },
	    'layer': { name: 'LayerElement', markdown: Layer }
	}
}
