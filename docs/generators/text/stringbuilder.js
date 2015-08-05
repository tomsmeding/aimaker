const MAX_LINE_LENGTH = 80;
const MAX_STOP_DIFF = 15;

'use strict';

function breakString (str, length) {
	var length = length || MAX_LINE_LENGTH;
	var chars  = '!?:;.-';

	var stopIndexes  = [];
	var spaceIndexes = [];

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
		} else if (char == ' ' || char == '\t') {
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
		} else if (linelength + 1 < length) {
			linelength++;
		} else if (closestStop[1] < MAX_STOP_DIFF) { // cut on nearby previous stop
			splits.push(closestStop[0]);
			linelength = i - closestStop[0];
		} else if (closestSpace[1] < Infinity) { // cut on previous space, if it exists.
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

	line = line.trim();
	if (line.length) res.push(line);
	return res;
}

function capFirst (str) {
	return str[0].toUpperCase() + str.substr(1);
}

module.exports = (function () {
	String.prototype.repeat = function (times) {
		var s = '';
		for (var i = 0; i < times; i++) {
			s += this;
		}
		return s;
	};

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

	StringBuilder.prototype.addObject = function (obj, table) {
		var self = this;
		var pairs = [];
		var maxKeySize = 0;

		for (var key in obj) {
			var val = obj[key];

			if (Array.isArray(val)) {
				if (key === 'arguments') {
					val = val.map(function (s) {
						return '\n- ' + s;
					}).join('').trim();
				} else {
					val = val.join('/');
				}
			} else if (typeof val !== 'string') {
				val = val.toString();
			}

			pairs.push([key, val]);
			if (key.length > maxKeySize) {
				maxKeySize = key.length;
			}
		}

		// Object is empty, nothing to print.
		if (pairs.length === 0) return;

		if (table) {
			self.add('| | |');
			self.add('|-----------:|:------------|');
			pairs.forEach(function (pair) {
				self.add('|' + capFirst(pair[0]) + '|' + pair[1].replace(/\n/g, '<br>') + '|');
			});
		} else {
			pairs.forEach(function (pair) {
				var s = capFirst(pair[0]) + ': ';

				var padding = '';
				for (var i = 0; i < maxKeySize + 2 - s.length; i++) {
					padding += ' ';
				}
				s = padding + s;

				var rightLines = breakString(pair[1], MAX_LINE_LENGTH - s.length);
				s += rightLines[0];
				rightLines.slice(1).forEach(function (line) {
					s += '\n';
					for (var i = 0; i < maxKeySize + 2; i++) {
						s += ' ';
					}
					s += line;
				});

				self.add(s);
			});
		}
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

	StringBuilder.prototype.addSection = function (str, borders) {
		var self = this;
		var borders = borders != null ? borders : false;
		var lines = breakString(str);

		var longest = lines.map(function (l) {
			return l.length;
		}).sort().reverse()[0];

		if (borders) self.add('-'.repeat(longest));
		lines.forEach(function (line) {
			self.add(line);
		});
		if (borders) self.add('-'.repeat(longest));
	};

	StringBuilder.prototype.toString = function () {
		return this._str.trim();
	};

	return StringBuilder;
})();
