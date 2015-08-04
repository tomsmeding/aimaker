var StringBuilder = require('./stringbuilder.js');

module.exports = function (ref, commit, outputType) {
	var ss = new StringBuilder();

	ss.addTitle('Aimaker Reference (commit ', commit, ')');

	ref.forEach(function (x) {
		if (x.subtitle) {
			ss.addSubTitle(x.subtitle);
			return; // prevent printing a triple '\n'.
		} else if (x.content) {
			ss.addSection(x.content, outputType === 'text');
		} else {
			ss.addObject(x, outputType === 'markdown');
		}

		ss.add();
	});

	return ss.toString();
};
