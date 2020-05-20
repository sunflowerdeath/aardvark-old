const Alignment = {
    topLeft(top, left) {
        return { origin: AlignmentOrigin.topLeft, vert: top, horiz: left }
    },
    topRight(top, right) {
        return { origin: AlignmentOrigin.topRight, vert: top, horiz: right }
    },
    bottomLeft(bottom, left) {
        return { origin: AlignmentOrigin.bottomLeft, vert: bottom, horiz: left }
    },
    bottomRight(bottom, right) {
        return {
            origin: AlignmentOrigin.bottomRight,
            vert: bottom,
            horiz: right
        }
    }
}

export { Alignment }
