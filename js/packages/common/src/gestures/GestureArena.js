import GestureArenaEntry from './GestureArenaEntry.js'

class GestureArena {
    constructor() {
        this.isClosed = false
        this.isResolved = false
        this.entries = []
    }

    close() {
        this.isClosed = true
        this.checkLastUnresolvedEntry()
    }

    resolve() {
        if (this.isResolved) return
        this.isResolved = true
        this.entries.map((entry, index) => entry.onResolve(index === 0))
    }

    checkLastUnresolvedEntry() {
        const unresolved = this.entries.filter(entry => !entry.isResolved)
        if (unresolved.length === 1) {
            this.isResolved = true
            unresolved.onResolve(true)
        }
    }

    addEntry(handler) {
        if (this.isClosed) return
        
        const entry = new GestureArenaEntry(this, handler)
        this.entries.push(entry)
        return entry
    }

    acceptEntry(acceptedEntry) {
        if (this.isResolved) return

        this.isResolved = true
        this.entries(entry => entry.onResolve(entry === acceptedEntry))
    }

    rejectEntry(rejectedEntry) {
        if (this.isResolved) return

        this.entries(entry => {
            if (entry === rejectedEntry) entry.onResolve(false)
        })
        this.checkLastUnresolvedEntry()
    }
}

export default GestureArena