import React from 'react'

import Index from './index.md'

import Document from '../../generated/Document.md'
import Element from '../../generated/Element.md'

import Size from '../../generated/Size.md'
import Position from '../../generated/Position.md'
import ValueType from '../../generated/ValueType.md'
import Value from '../../generated/Value.md'
import Color from '../../generated/Color.md'
import Connection from '../../generated/Connection.md'

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

const BaseTypes = () => <>
    <Size/>
    <Position/>
    <Color/>
    <ValueType/>
    <Value/>
    <Connection/>
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

export default {
	name: 'JS API',
	stories: {
	    'index': { name: 'Index', markdown: Index },
	    'base-types': { name: 'Base types', markdown: BaseTypes },
	    'document': { name: 'Document', markdown: Document },
	    'element': { name: 'Element', markdown: Element },
	    'aligned-element': { name: 'AlignedElement', markdown: Aligned },
	    'background-element': { name: 'BackgroundElement', markdown: BackgroundElement },
	    'border-element': { name: 'BorderElement', markdown: Border },
	    'center-element': { name: 'CenterElement', markdown: CenterElement },
	    'clip-element': { name: 'ClipElement', markdown: ClipElement },
	    'custom-layout-element': { name: 'CustomLayoutElement', markdown: CustomLayoutElement },
	    'flex-element': { name: 'FlexElement', markdown: Flex }
	}
}
