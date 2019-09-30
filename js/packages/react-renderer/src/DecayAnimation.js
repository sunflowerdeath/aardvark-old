class DecayAnimation {
	constructor(config) {
		this.deceleration = config.deceleration ?? 0.998
		this.velocity = config.velocity
	}

	start({ fromValue, onUpdate, onEnd }) {
		this.fromValue = fromValue
		this.lastValue = fromValue
		this.onUpdate = onUpdate.bind(this)
		this.onEnd = onEnd
		this.isActive = true
		this.startTime = Date.now()
		this.animationFrame = requestAnimationFrame(this.onUpdate.bind(this))
	}

	onUpdate() {
		const now = Date.now()
        
		const value =
			this.fromValue +
			(this.velocity / (1 - this.deceleration)) *
				(1 -
					Math.exp(-(1 - this.deceleration) * (now - this.startTime)))

		this.onUpdate(value)

		if (Math.abs(this.lastValue - value) < 0.1) {
			this.isActive = false
			this.callOnEnd({ finished: true })
			return
		}

		this.lastValue = value

		if (this.isActive) {
			this.animationFrame = requestAnimationFrame(this.onUpdate)
		}
	}

	stop() {
		this.isActive = false
		cancelAnimationFrame(this.animationFrame)
		this.callOnEnd({ finished: false })
	}

	callOnEnd(result) {
		const onEnd = this.onEnd
		this.onEnd = undefined
		if (onEnd) onEnd(result)
	}
}

module.exports = DecayAnimation
