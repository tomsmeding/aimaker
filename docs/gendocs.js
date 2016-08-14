#!/usr/bin/env node

'use strict';

var fs     = require('fs');
var path   = require('path');
var ref    = require('./docs.json');
var commit = fs.readFileSync(path.join(__dirname, '../.git/refs/heads/master'), { encoding: 'ascii' }).trim().substr(0, 7);

var args = process.argv.slice(2);
var getArg = function (flag, short) {
	var flagIndex  = args.indexOf('--' + flag);
	var shortIndex = args.indexOf('-'  + short);

	if      (flagIndex > -1)  return args[ flagIndex + 1];
	else if (shortIndex > -1) return args[shortIndex + 1];
};

var outputLocation = getArg('output', 'o') || '';
var outputType     = getArg('type'  , 't') || 'text';

var generatorsDir = path.join(__dirname, '/generators/');
var generators = fs.readdirSync(generatorsDir);
var module = generators.map(function (generator) {
	return require(path.join(generatorsDir, generator));
}).find(function (module) {
	return module.TYPES.indexOf(outputType) > -1;
});

if (module === undefined) {
	throw new Error(`no module supporting outputType '${outputType}' found`);
}

// outputType is given to support generators which handle multiple output
// types, for example, the text generator handles both text and markdown.
var res = module(ref, commit, outputType, getArg);

if (outputLocation.trim().length > 0) {
	fs.writeFileSync(outputLocation, res);
} else {
	console.log(res);
}
