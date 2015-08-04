#!/usr/bin/env node

(function () {
	'use strict';

	var fs            = require('fs');
	var ref           = require('./docs.json');
	var StringBuilder = require('./stringbuilder.js');
	var commit        = fs.readFileSync('../.git/refs/heads/master', { encoding: 'ascii' }).trim().substr(0, 7);

	var args = process.argv.slice(2);
	var getArg = function (flag, short) {
		var flagIndex  = args.indexOf('--' + flag);
		var shortIndex = args.indexOf('-'  + short);

		if (flagIndex > -1) return args[flagIndex + 1];
		else if (shortIndex > -1) return args[shortIndex + 1];
		else return '';
	};

	var outputLocation = getArg('output', 'o') || '';
	var outputType     = getArg('type'  , 't') || 'text';

	var ss = new StringBuilder();
	if (outputType === 'text' || outputType === 'markdown') {
		ss.addTitle('Aimaker Reference (commit ', commit, ')');

		ref.forEach(function (x) {
			if (x.subtitle) {
				ss.addSubTitle(x.subtitle);
				return; // prevent printing a triple '\n'.
			} else if (x.content) {
				ss.addSection(x.content);
			} else {
				ss.addObject(x);
			}

			ss.add();
		});

		if (outputLocation.trim().length > 0) {
			fs.writeFileSync(outputLocation, ss.toString());
		} else {
			console.log(ss.toString());
		}
	} else if (outputType === 'html') {
		console.warn('not impmplemented.');
	} else {
		throw new Error("Output type '" + outputType + "' unknown.");
	}
})();
