import React, { useState, useRef, useCallback, useEffect } from 'react'
import PropTypes from 'prop-types'
import Animated from 'animated'
import HoverRecognizer from '@advk/common/src/gestures/HoverRecognizer.js'
import DragRecognizer from '@advk/common/src/gestures/DragRecognizer.js'
import MultiRecognizer from '@advk/common/src/gestures/MultiRecognizer.js'
import RawRecognizer from '@advk/common/src/gestures/RawRecognizer.js'
import VelocityTracker from '@advk/common/src/gestures/VelocityTracker.js'
import KeyCode from '@advk/common/src/events/KeyCode.js'
import isMouseButtonPress from '@advk/common/src/events/isMouseButtonPress.js'
import { Responder } from '../nativeComponents'
import useContext from '../hooks/useContext.js'
import Scrolled from './Scrolled.js'

const OverscrollResistance = {
    none: value => value,
    full: value => 0,
    diminishing: value => Math.pow(value, 0.8)
}

const removeHandler = (object, key) => {
    if (key in object) {
        object[key]()
        delete object[key]
    }
}

const getMaxScrollTop = ctx => {
    const elem = ctx.elemRef.current
    return Math.max(elem.scrollHeight - elem.height, 0)
}

const isScrollable = ctx => !ctx.props.isDisabled && getMaxScrollTop(ctx) > 0

const clampScrollTop = (ctx, scrollTop) =>
    Math.min(Math.max(scrollTop, 0), getMaxScrollTop(ctx))

const animateOverscroll = ctx => {
    const scrollTop = ctx.scrollTopValue.__getValue()
    const toValue = clampScrollTop(ctx, scrollTop)
    if (toValue === scrollTop) return false
    Animated.spring(ctx.scrollTopValue, {
        toValue,
        bounciness: ctx.props.dragOverscrollBounciness,
        // Here velocity is based on seconds instead of milliseconds
        velocity: ctx.velocityTracker.getVelocity() * 1000
    }).start()
    return true
}

const animateScroll = (ctx, nextScrollTop) => {
    const scrollTop = ctx.scrollTopValue.__getValue()
    if (nextScrollTop === scrollTop) return
    Animated.spring(ctx.scrollTopValue, {
        toValue: nextScrollTop,
        overshootClamping: true
        // TODO configure speed
    }).start()
}

const checkOverscroll = ctx => {
    const scrollTop = ctx.scrollTopValue.__getValue()
    const maxScrollTop = getMaxScrollTop(ctx)
    if (scrollTop > maxScrollTop) ctx.scrollTopValue.setValue(maxScrollTop)
}

const checkDisabled = ctx => {
    const { state, props, elemRef } = ctx
    const { recognizer, height, scrollHeight } = state
    const elem = elemRef.current
    const isDisabled = props.isDisabled || scrollHeight <= height
    isDisabled ? recognizer.disable() : recognizer.enable()
}

// Keyboard

// Which Scrollable currently handles key events
let currentKeyHandler

const makeCurrentKeyHandler = ctx => {
    if (currentKeyHandler) currentKeyHandler.removeKeyHandler()
    const document = ctx.elemRef.current.document
    ctx.removeKeyHandler = document.addKeyHandler(onKeyEvent(ctx, ?))
    currentKeyHandler = ctx
}

// When there are multiple nested scrollables, only top one should become active
let alreadySetKeyHandler = false

const onRawEvent = (ctx, event) => {
    if (!isScrollable(ctx)) return
    if (!alreadySetKeyHandler && isMouseButtonPress(event)) {
        makeCurrentKeyHandler(ctx)
        alreadySetKeyHandler = true
        setTimeout(() => {
            alreadySetKeyHandler = false
        }, 0)
    }
}

const onKeyEvent = (ctx, event) => {
    if (!isScrollable(ctx)) return
    if (event.action === KeyAction.press || event.action === KeyAction.repeat) {
        if (event.key === KeyCode.UP || event.key == KeyCode.DOWN) {
            const scrollTop = ctx.scrollTopValue.__getValue()
            const delta =
                (event.key === KeyCode.release ? -1 : 1) *
                ctx.props.keyboardScrollSpeed
            animateScroll(ctx, clampScrollTop(ctx, scrollTop + delta))
        } else if (event.key === KeyCode.HOME) {
            animateScroll(ctx, 0)
        } else if (event.key === KeyCode.END) {
            animateScroll(ctx, getMaxScrollTop(ctx))
        } else if (
            event.key === KeyCode.PAGE_UP ||
            event.key === KeyCode.PAGE_DOWN
        ) {
            const scrollTop = ctx.scrollTopValue.__getValue()
            const delta =
                (event.key === KeyCode.PAGE_UP ? -1 : 1) *
                ctx.elemRef.current.height
            animateScroll(ctx, clampScrollTop(ctx, scrollTop + delta))
        }
    }
}

// Mouse wheel
const onHoverStart = ctx => {
    const document = ctx.elemRef.current.document
    ctx.removeScrollHandler = document.addScrollHandler(onScrollEvent(ctx, ?))
}

const onHoverEnd = ctx => removeHandler(ctx, 'removeScrollHandler')

// When there are multiple nested scrollables, only top one should be scrolled
const alreadyScrolled = false

const onScrollEvent = (ctx, event) => {
    if (!isScrollable(ctx) || alreadyScrolled) return
    const scrollTop = ctx.scrollTopValue.__getValue()
    const nextScrollTop = clampScrollTop(
        ctx,
        scrollTop + -1 * event.top * ctx.props.mousewheelScrollSpeed
    )
    animateScroll(ctx, nextScrollTop)
    alreadyScrolled = true
    setTimeout(() => {
        alreadyScrolled = false
    }, 0)
}

// Touch
const onDragStart = (ctx, event) => {
    if (!isScrollable(ctx)) return
    ctx.initialScrollTop = ctx.scrollTopValue.__getValue()
}

const onDragMove = (ctx, event) => {
    const maxScrollTop = getMaxScrollTop(ctx)
    let nextScrollTop = ctx.initialScrollTop - event.deltaTop
    const overscrollResistance = ctx.props.dragOverscrollResistance
    if (nextScrollTop < 0) {
        nextScrollTop = -overscrollResistance(-nextScrollTop)
    } else if (nextScrollTop > maxScrollTop) {
        nextScrollTop =
            maxScrollTop + overscrollResistance(nextScrollTop - maxScrollTop)
    }
    ctx.scrollTopValue.setValue(nextScrollTop)
}

const onDragEnd = (ctx, event) => {
    if (animateOverscroll(ctx)) return

    ctx.decayListener = ctx.scrollTopValue.addListener(() =>
        animateOverscroll(ctx)
    )
    Animated.decay(ctx.scrollTopValue, {
        velocity: -event.velocity,
        deceleration: ctx.props.dragDecayDeceleration
    }).start(({ finished }) => {
        ctx.scrollTopValue.removeListener(ctx.decayListener)
    })
}

const unmount = ctx => {
    ctx.scrollTopValue.stopAnimation()
    removeHandler(ctx, 'removeScrollHandler')
    removeHandler(ctx, 'removeKeyHandler')
    if (currentKeyHandler === ctx) {
        currentKeyHandler = undefined
        removeHandler(ctx, 'removeKeyHandler')
    }
}

const renderContent = (ctx, content) => {
    const { props, state, scrollTopValue } = ctx
    const { isDisabled, wrapper } = props
    if (!wrapper) return content
    const { height, scrollHeight } = state
    return React.createElement(wrapper, {
        children: content,
        isDisabled,
        height,
        scrollHeight,
        scrollTopValue
    })
}

const Scrollable = props => {
    const elemRef = useRef()
    const ctx = useContext({
        props,
        initialCtx: () => ({
            elemRef,
            scrollTopValue: new Animated.Value(0),
            velocityTracker: new VelocityTracker()
        }),
        initialState: ctx => ({
            recognizer: new MultiRecognizer({
                hover: new HoverRecognizer({
                    onHoverStart: onHoverStart(ctx, ?),
                    onHoverEnd: onHoverEnd(ctx, ?)
                }),
                drag: new DragRecognizer({
                    document: () => ctx.elemRef.current.document,
                    onDragStart: onDragStart(ctx, ?),
                    onDragMove: onDragMove(ctx, ?),
                    onDragEnd: onDragEnd(ctx, ?)
                }),
                raw: new RawRecognizer({
                    handler: onRawEvent(ctx, ?)
                })
            })
        })
    })
    useEffect(() => {
        ctx.scrollTopValue.addListener(({ value }) => {
            elemRef.current.scrollTop = Math.round(value)
            ctx.velocityTracker.addPoint(Date.now(), value)
        })
        return unmount.bind(null, ctx)
    }, [])
    useEffect(() => checkDisabled(ctx), [
        ctx.state.height,
        ctx.state.scrollHeight,
        ctx.props.isDisabled
    ])
    useEffect(() => checkOverscroll(ctx), [
        ctx.state.height,
        ctx.state.scrollHeight
    ])
    const content = (
        <Scrolled
            ref={ctx.elemRef}
            onChangeHeight={useCallback(() =>
                ctx.setState({ height: ctx.elemRef.current.height })
            )}
            onChangeScrollHeight={useCallback(() =>
                ctx.setState({
                    scrollHeight: ctx.elemRef.current.scrollHeight
                })
            )}
        >
            {ctx.props.children}
        </Scrolled>
    )
    return (
        <Responder handler={useCallback(ctx.state.recognizer.getHandler())}>
            {renderContent(ctx, content)}
        </Responder>
    )
}

Scrollable.propTypes = {
    isDisabled: PropTypes.bool.isRequired,
    dragOverscrollBounciness: PropTypes.number.isRequired,
    dragOverscrollResistance: PropTypes.func.isRequired,
    dragDecayDeceleration: PropTypes.number.isRequired,
    keyboardScrollSpeed: PropTypes.number.isRequired,
    mousewheelScrollSpeed: PropTypes.number.isRequired,
    contentWrapper: PropTypes.func
}

Scrollable.defaultProps = {
    isDisabled: false,
    dragOverscrollBounciness: 6,
    dragOverscrollResistance: OverscrollResistance.diminishing,
    dragDecayDeceleration: 0.998,
    keyboardScrollSpeed: 50,
    mousewheelScrollSpeed: 100
}

export { OverscrollResistance }
export default Scrollable
