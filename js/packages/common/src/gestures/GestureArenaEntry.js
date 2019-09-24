class GestureArenaEntry {
    constructor(arena, handler) {
        this.arena = arena
        this.handler = handler
        this.isResolved = false
        this.isAccepted = false
    }

    // Called when arena resolves this entry
    onResolve(isAccepted) {
        if (this.isResolved) return

        this.isResolved = true
        this.isAccepted = isAccepted
        this.handler(isAccepted)
    }

    // Ask arena to reject this entry
    reject() {
        if (!this.isResolved) this.arena.rejectEntry(this)
    }

    // Ask arena to accept this entry and reject all others
    accept() {
        if (!this.isResolved) this.arena.acceptEntry(this)
    }
}

export default GestureArenaEntry