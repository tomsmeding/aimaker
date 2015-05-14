#include <string>
#include "parser.h"
#include "util.h"

namespace Parser {
	Instruction convinstr(string word) {
		if (word == "move") return INSTR_MOVE;
		else if (word == "sto") return INSTR_STO;
	}

	/**
	 * Takes the given `lines` and returns a parsed result of it.
	 * @param lines {const vector<string>} The lines of code to parse.
	 * @return {ParseResult}
	 */
	ParseResult parser(const vector<string> lines) {
		int currPage;
		ParseResult parseResult;

		for (int i = 0; i < (int)lines.size(); i++) {
			string line = lines[i];
			string trimmed = trim(line);

			if (trimmed.find("#page") == 0) { // page
				parseResult.pages.push_back(* new Codepage());
				currPage++;
			} else if (trimmed[trimmed.size() - 1] == ':') { // label
				string labelName = trimmed.substr(0, trimmed.size() - 1);
				pair<int, int> position = make_pair(currPage, i);

				parseResult.labels.emplace(make_pair(labelName, position));
			} else {
				string functionName = split(trimmed, ' ')[0];
				convinstr()
			}
		}

		return parseResult;
	}
}
