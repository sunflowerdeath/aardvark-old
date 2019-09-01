/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

const resolveDotsInPath = path => {
	if (!path) return path

	if (path.indexOf('./') === -1) return path

	var result = []

	var components = path.split('/')
	for (var i = 0; i < components.length; ++i) {
		var component = components[i]

		// Skip over "./".
		if (component === '.') continue

		// Rewind one component for "../".
		if (component === '..') {
			if (result.length === 1) continue
			result.pop()
			continue
		}

		result.push(component)
	}

	return result.join('/')
}

const parseURL = url => {
	url = url ? url.trim() : ''

	if (url.startsWith('data:'))
		return {
			scheme: 'data',
			userinfo: null,
			host: null,
			port: null,
			path: null,
			queryString: null,
			fragment: null,
			lastPathComponent: null
		}

	let match = url.match(
		/^(?<scheme>[^\/:]+):\/\/(?:(?<userinfo>[^#@\/]+)@)?(?<host>[^\/#:]*)(?::(?<port>[\d]+))?(?:(?<path>\/[^#]*)?(?:#(?<fragment>.*))?)?$/i
	)
	if (!match) {
		return {
			scheme: null,
			userinfo: null,
			host: null,
			port: null,
			path: null,
			queryString: null,
			fragment: null,
			lastPathComponent: null
		}
	}

	let scheme = match.groups.scheme.toLowerCase()
	let userinfo = match.groups.userinfo || null
	let host = match.groups.host.toLowerCase()
	let port = Number(match.groups.port) || null
	let wholePath = match.groups.path || null
	let fragment = match.groups.fragment || null
	let path = wholePath
	let queryString = null

	// Split the path and the query string.
	if (wholePath) {
		let indexOfQuery = wholePath.indexOf('?')
		if (indexOfQuery !== -1) {
			path = wholePath.substring(0, indexOfQuery)
			queryString = wholePath.substring(indexOfQuery + 1)
		}
		path = resolveDotsInPath(path)
	}

	// Find last path component.
	let lastPathComponent = null
	if (path && path !== '/') {
		// Skip the trailing slash if there is one.
		let endOffset = path[path.length - 1] === '/' ? 1 : 0
		let lastSlashIndex = path.lastIndexOf('/', path.length - 1 - endOffset)
		if (lastSlashIndex !== -1) {
			lastPathComponent = path.substring(
				lastSlashIndex + 1,
				path.length - endOffset
			)
		}
	}

	return {
		scheme,
		userinfo,
		host,
		port,
		path,
		queryString,
		fragment,
		lastPathComponent
	}
}

const absoluteURL = (partialURL, baseURL) => {
	partialURL = partialURL ? partialURL.trim() : ''

	// Return data and javascript URLs as-is.
	if (
		partialURL.startsWith('data:') ||
		partialURL.startsWith('javascript:') ||
		partialURL.startsWith('mailto:')
	)
		return partialURL

	// If the URL has a scheme it is already a full URL, so return it.
	if (parseURL(partialURL).scheme) return partialURL

	// If there is no partial URL, just return the base URL.
	if (!partialURL) return baseURL || null

	var baseURLComponents = parseURL(baseURL)

	// The base URL needs to be an absolute URL. Return null if it isn't.
	if (!baseURLComponents.scheme) return null

	// A URL that starts with "//" is a full URL without the scheme.
	// Use the base URL scheme.
	if (partialURL[0] === '/' && partialURL[1] === '/') {
		return baseURLComponents.scheme + ':' + partialURL
	}

	// The path can be null for URLs that have just a scheme and host
	// (like "http://apple.com"). So make the path be "/".
	if (!baseURLComponents.path) baseURLComponents.path = '/'

	// Generate the base URL prefix that is used in the rest of the cases.
	var baseURLPrefix =
		baseURLComponents.scheme +
		'://' +
		baseURLComponents.host +
		(baseURLComponents.port ? ':' + baseURLComponents.port : '')

	// A URL that starts with "?" is just a query string that gets applied to
	// the base URL (replacing the base URL query string and fragment).
	if (partialURL[0] === '?') {
		return baseURLPrefix + baseURLComponents.path + partialURL
	}

	// A URL that starts with "/" is an absolute path that gets applied to the
	// base URL (replacing the base URL path, query string and fragment).
	if (partialURL[0] === '/') {
		return baseURLPrefix + resolveDotsInPath(partialURL)
	}

	// A URL that starts with "#" is just a fragment that gets applied to the
	// base URL (replacing the base URL fragment, maintaining the query string).
	if (partialURL[0] === '#') {
		let queryStringComponent = baseURLComponents.queryString
			? '?' + baseURLComponents.queryString
			: ''
		return (
			baseURLPrefix +
			baseURLComponents.path +
			queryStringComponent +
			partialURL
		)
	}

	// Generate the base path that is used in the final case by removing
	// everything after the last "/" from the base URL's path.
	var basePath =
		baseURLComponents.path.substring(
			0,
			baseURLComponents.path.lastIndexOf('/')
		) + '/'
	return baseURLPrefix + resolveDotsInPath(basePath + partialURL)
}

class StringCharIterator {
	constructor(string) {
		this.string = string
		this.position = 0
	}

	next() {
		return this.string[this.position++]
	}

	peek() {
		return this.string[this.position]
	}

	hasNext() {
		return this.position < this.string.length
	}
}

const base64Digits =
	'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
const base64Map = {}
for (let i = 0; i < base64Digits.length; ++i) {
	base64Map[base64Digits[i]] = i
}

const VLQ_BASE_SHIFT = 5
const VLQ_BASE_MASK = (1 << 5) - 1
const VLQ_CONTINUATION_MASK = 1 << 5

const decodeVLQ = stringCharIterator => {
	// Read unsigned value.
	var result = 0
	var shift = 0
	do {
		var digit = base64Map[stringCharIterator.next()]
		result += (digit & VLQ_BASE_MASK) << shift
		shift += VLQ_BASE_SHIFT
	} while (digit & VLQ_CONTINUATION_MASK)

	// Fix the sign.
	var negative = result & 1
	result >>= 1
	return negative ? -result : result
}

const isSeparator = char => char === ',' || char === ';'

class SourceMap {
	constructor(sourceMappingURL, payload) {
		this._sourceMappingURL = sourceMappingURL
		this._reverseMappingsBySourceURL = {}
		this._mappings = []
		this._sources = {}
		this._sourceRoot = null
		this._sourceContentByURL = {}
		this.parseMappingPayload(payload)
	}

	parseMappingPayload(mappingPayload) {
		if (mappingPayload.sections) {
			this.parseSections(mappingPayload.sections)
		} else {
			this.parseMap(mappingPayload, 0, 0)
		}
	}

	parseSections(sections) {
		for (var i = 0; i < sections.length; ++i) {
			var section = sections[i]
			this.parseMap(
				section.map,
				section.offset.line,
				section.offset.column
			)
		}
	}

	parseMap(map, lineNumber, columnNumber) {
		var sourceIndex = 0
		var sourceLineNumber = 0
		var sourceColumnNumber = 0
		var nameIndex = 0

		var sources = []
		var originalToCanonicalURLMap = {}
		for (var i = 0; i < map.sources.length; ++i) {
			var originalSourceURL = map.sources[i]
			var href = originalSourceURL
			if (map.sourceRoot && href.charAt(0) !== '/') {
				href = map.sourceRoot.replace(/\/+$/, '') + '/' + href
			}
			var url = absoluteURL(href, this._sourceMappingURL) || href
			originalToCanonicalURLMap[originalSourceURL] = url
			sources.push(url)
			this._sources[url] = true

			if (map.sourcesContent && map.sourcesContent[i]) {
				this._sourceContentByURL[url] = map.sourcesContent[i]
			}
		}

		this._sourceRoot = map.sourceRoot || null

		var stringCharIterator = new StringCharIterator(map.mappings)
		var sourceURL = sources[sourceIndex]

		while (true) {
			if (stringCharIterator.peek() === ',') {
				stringCharIterator.next()
			} else {
				while (stringCharIterator.peek() === ';') {
					lineNumber += 1
					columnNumber = 0
					stringCharIterator.next()
				}
				if (!stringCharIterator.hasNext()) break
			}

			columnNumber += decodeVLQ(stringCharIterator)
			if (isSeparator(stringCharIterator.peek())) {
				this._mappings.push([lineNumber, columnNumber])
				continue
			}

			var sourceIndexDelta = decodeVLQ(stringCharIterator)
			if (sourceIndexDelta) {
				sourceIndex += sourceIndexDelta
				sourceURL = sources[sourceIndex]
			}
			sourceLineNumber += decodeVLQ(stringCharIterator)
			sourceColumnNumber += decodeVLQ(stringCharIterator)
			if (!isSeparator(stringCharIterator.peek())) {
				nameIndex += decodeVLQ(stringCharIterator)
			}

			this._mappings.push([
				lineNumber,
				columnNumber,
				sourceURL,
				sourceLineNumber,
				sourceColumnNumber
			])
		}

		for (var i = 0; i < this._mappings.length; ++i) {
			var mapping = this._mappings[i]
			var url = mapping[2]
			if (!url) continue
			if (!this._reverseMappingsBySourceURL[url]) {
				this._reverseMappingsBySourceURL[url] = []
			}
			var reverseMappings = this._reverseMappingsBySourceURL[url]
			var sourceLine = mapping[3]
			if (!reverseMappings[sourceLine]) {
				reverseMappings[sourceLine] = [mapping[0], mapping[1]]
			}
		}
	}

	findEntry(lineNumber, columnNumber) {
		var first = 0
		var count = this._mappings.length
		while (count > 1) {
			var step = count >> 1
			var middle = first + step
			var mapping = this._mappings[middle]
			if (
				lineNumber < mapping[0] ||
				(lineNumber === mapping[0] && columnNumber < mapping[1])
			) {
				count = step
			} else {
				first = middle
				count -= step
			}
		}
		var entry = this._mappings[first]
		if (
			!first &&
			entry &&
			(lineNumber < entry[0] ||
				(lineNumber === entry[0] && columnNumber < entry[1]))
		) {
			return null
		}
		return entry
	}

	findEntryReversed(sourceURL, lineNumber) {
		var mappings = this._reverseMappingsBySourceURL[sourceURL]
		for (; lineNumber < mappings.length; ++lineNumber) {
			var mapping = mappings[lineNumber]
			if (mapping) return mapping
		}
		return this._mappings[0]
	}
}

export default SourceMap
