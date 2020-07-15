import React from 'react'
import { Background, Border, Sized, Padded } from '../nativeComponents'

const Container = props => {
    const {
        padding,
        margin,
        sizeConstraints,
        borders,
        radiuses,
        background,
        children
    } = props
    let result = children
    if (padding !== undefined) {
        result = <Padded padding={padding}>{result}</Padded>
    }
    if (background !== undefined)
        result = <Background color={background}>{result}</Background>
    if (borders !== undefined || radiuses !== undefined) {
        result = (
            <Border borders={borders} radiuses={radiuses}>
                {result}
            </Border>
        )
    }
    if (sizeConstraints !== undefined) {
        result = <Sized sizeConstraints={sizeConstraints}>{result}</Sized>
    }
    if (margin !== undefined) {
        result = <Padded padding={margin}>{result}</Padded>
    }
    return result
}

export default Container
