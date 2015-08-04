var StringBuilder = require('./stringbuilder.js');

module.exports = function (ref, commit) {
	var ss = new StringBuilder();

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

	return ss.toString();
};
