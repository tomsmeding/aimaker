#!/usr/bin/env node

(function () {
	"use strict";

	var fs            = require('fs');
	var ref           = require('./docs.json');
	var StringBuilder = require('./stringbuilder.js');
	var commit        = fs.readFileSync('../.git/refs/heads/master', { encoding: 'ascii' }).replace(/\n$/, '').substr(0, 6);

	var args = process.argv.slice(2);
	var getArg = function (flag, short) {
		var flagIndex  = args.indexOf('--' + flag);
		var shortIndex = args.indexOf('-'  + short);

		return (flagIndex > -1) ? args[flagIndex + 1] : args[shortIndex + 1];
	};

	var outputLocation = getArg('output', 'o') || '';
	var outputType     = getArg('type'  , 't') || 'text';

	var ss = new StringBuilder();
	if (outputType === 'text' || outputType === 'markdown') {
		var printText = function (str) {
			ss.add('-'.repeat(str.length));
			ss.add(str);
			ss.add('-'.repeat(str.length));
		};

		ss.addTitle('Aimaker Reference (commit ', commit, ')');

		ref.forEach(function (x) {
			if (x.subtitle) {
				ss.addSubTitle(x.subtitle);
				return; // prevent printing a triple '\n'.
			} else if (x.content) {
				printText(x.content);
			} else {
				ss.addObject(x);
			}

			ss.add();
		});

		console.log(ss.toString());
	} else if (outputType === 'html') {
		console.warn('not impmplemented.');
	} else {
		throw new Error("Ouput type '" + outputType + "' unknown.");
	}
})();
