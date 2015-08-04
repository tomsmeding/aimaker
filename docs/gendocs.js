#!/usr/bin/env node

(function () {
	'use strict';

	var fs     = require('fs');
	var ref    = require('./docs.json');
	var commit = fs.readFileSync('../.git/refs/heads/master', { encoding: 'ascii' }).trim().substr(0, 7);

	var args = process.argv.slice(2);
	var getArg = function (flag, short) {
		var flagIndex  = args.indexOf('--' + flag);
		var shortIndex = args.indexOf('-'  + short);

		if      (flagIndex > -1)  return args[ flagIndex + 1];
		else if (shortIndex > -1) return args[shortIndex + 1];
		else return '';
	};

	var outputLocation = getArg('output', 'o') || '';
	var outputType     = getArg('type'  , 't') || 'text';

	if (outputType === 'text' || outputType === 'markdown') {
		var module = require('./generators/text');
	} else {
		var module = require('./generators/' + outputType);
	}

	var s = module(ref, commit);

	if (outputLocation.trim().length > 0) {
		fs.writeFileSync(outputLocation, s);
	} else {
		console.log(s);
	}
})();
