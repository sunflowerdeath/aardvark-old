const SourceMap = require('./SourceMap.js')

const getOriginalLocation = (location, sourceMapStr) => {
	const parsedSourceMap = JSON.parse(sourceMapStr)
	const sourceMap = new SourceMap(location.sourceURL, parsedSourceMap)
	const [_1, _2, sourceURL, line, column] = sourceMap.findEntry(
		location.line,
		location.column
	)
	return { sourceURL, line, column }
}

module.exports = getOriginalLocation
