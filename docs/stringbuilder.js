const MAX_LINE_LENGTH = 65;
const MAX_DIFF = 10;

function breakString (str, length) {
	var length = length || MAX_LINE_LENGTH;
	var chars  = '!?:;.,';

	var stopIndexes    = [];
	var spaceIndexes   = [];

	var closest = function (arr, val) {
		var closestVal = -1;
		var diff = Infinity;

		arr.forEach(function (x) {
			var newdiff = val - x;

			if (newdiff >= 0 && newdiff < diff) {
				closestVal = x;
				diff = newdiff;
			}
		});

		return [ closestVal, diff ];
	};

	for (var i = 0; i < str.length; i++) {
		var char = str[i];

		if (chars.indexOf(char) > -1) {
			stopIndexes.push(i);
		} else if (char == ' ') {
			spaceIndexes.push(i);
		}
	}

	var linelength = 0;
	var splits = [];
	for (var i = 0; i < str.length; i++) {
		var char         = str[i];
		var closestStop  = closest(stopIndexes, i);
		var closestSpace = closest(spaceIndexes, i);

		if (char === '\n') {
			splits.push(i);
			linelength = 0;
			continue;
		}

		if (linelength + 1 < length) {
			linelength++;
			continue;
		} else if (closestStop[1] < MAX_DIFF + 5) { // cut on nearby stop
			splits.push(closestStop[0]);
			linelength = i - closestStop[0];
		} else if (closestSpace[1] < MAX_DIFF) { // cut on nearby space
			splits.push(closestSpace[0]);
			linelength = i - closestSpace[0];
		} else { // force cut
			splits.push(i);
			linelength = 0;
		}
	}

	var res = [];
	var line = '';
	for (var i = 0; i < str.length; i++) {
		line += str[i];

		if (splits.indexOf(i) > -1) {
			res.push(line.trim());
			line = '';
		}
	}

	line = line.trim()
	if (line.length) res.push(line);
	return res;
}

module.exports = (function () {
	"use strict";

	String.prototype.repeat = function (times) {
		var s = '';
		for (var i = 0; i < times; i++) {
			s += this;
		}
		return s;
	}

	var StringBuilder = function () {
		this._str = '';
	};

	StringBuilder.prototype.add = function (/* strings */) {
		var self = this;

		Array.prototype.slice.call(arguments).forEach(function (s) {
			self._str += s;
		});

		self._str += '\n';
	};

	StringBuilder.prototype.addObject = function (obj) {
		var self    = this;
		var pairs   = [];
		var largest = [ 0, 0 ]; // [ index, length ]

		for (var key in obj) {
			if (key.length > largest[1]) {
				largest = [ pairs.length, key.length ];
			}

			pairs.push([ key, obj[key] ]);
		}

		if (pairs.length === 0) return;
		var requiredLength = largest[1];

		pairs.forEach(function (p) {
			var left  = p[0][0].toUpperCase() + p[0].substr(1);
			var right = p[1];
			var rightLines = breakString(p[1]);

			var s = left + ': ';

			while (s.length < requiredLength + 2) {
				s += ' ';
			}

			s += rightLines[0];

			rightLines.slice(1).forEach(function (line) {
				s += '\n';
				for (var i = 0; i < requiredLength + 2; i++) {
					s += ' ';
				}
				s += line;
			});

			self.add(s);
		});
	};

	StringBuilder.prototype.addTitle = function () {
		var s = Array.prototype.slice.call(arguments).join('');
		this.add(s);
		this.add('='.repeat(s.length), '\n');
	};

	StringBuilder.prototype.addSubTitle = function () {
		var s = Array.prototype.slice.call(arguments).join('');
		this.add(s);
		this.add('-'.repeat(s.length), '\n');
	};

	StringBuilder.prototype.addSection = function (str) {
		var self = this;
		var lines = breakString(str);
		var longest = lines.map(function (l) { return l.length; }).sort().reverse()[0];

		self.add('-'.repeat(longest));
		lines.forEach(function (line) {
			self.add(line);
		});
		self.add('-'.repeat(longest));
	};

	StringBuilder.prototype.toString = function () {
		return this._str.trim();
	};

	return StringBuilder;
})();
