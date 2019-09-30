import PolynomialRegression from 'js-polynomial-regression'

// Duration without movement to assume that pointer stopped
const STOP_MOVE_TIME = 40

// How many last points should be used
const HISTORY_SIZE = 20

// How recent events should be used
const RECENT_TIME = 100

// Minimal amount of points required to calculate velocity
const MIN_SIZE = 3

// To get reliable velocity value, tracker stores some number of recent
// events and approximates them to function using least square regression,
// and velocity is a derivative of that function
class VelocityTracker {
	constructor() {
		this.points = []
	}

	addPoint(timestamp, value) {
		this.points.push({ timestamp, value })
		if (this.points.length > HISTORY_SIZE) this.points.unshift()
	}

	getVelocity() {
		if (this.points.length < MIN_SIZE) return 0

		const data = []
		const newestPoint = this.points[this.points.length - 1]
		let prevPoint = newestPoint
		for (let i = this.points.length - 1; i >= 0; i--) {
			const point = this.points[i]
            const age = newestPoint.timestamp - point.timestamp
			if (prevPoint.timestamp - point.timestamp > STOP_MOVE_TIME) break
			if (age > RECENT_TIME) break
			prevPoint = point
			data.push({ y: point.value, x: -age })
		}

		if (data.length < MIN_SIZE) return 0

		const model = PolynomialRegression.read(data, 2)
		const terms = model.getTerms()
		return terms[1]
	}
}

export default VelocityTracker
