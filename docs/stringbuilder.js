const MAX_LINE_LENGTH = 45;

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
			var splitted = p[1].split('\n');
			var rightLines = [];

			splitted.forEach(function (l) {
				if (l.length < MAX_LINE_LENGTH) {
					rightLines.push(l);
				} else {
					var arr = [];
					var left = l;
					var i = 0;

					while (left.length > 0) {
						arr.push(left.substr(i, MAX_LINE_LENGTH));
						left = left.substr(i + MAX_LINE_LENGTH);
						i += MAX_LINE_LENGTH;
					}

					rightLines = rightLines.concat(arr);
				}
			});

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

	StringBuilder.prototype.toString = function () {
		return this._str.trim();
	};

	return StringBuilder;
})();
