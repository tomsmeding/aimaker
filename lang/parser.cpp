#include <string>
#include <cstdio>
#include "parser.h"
#include "../util.h"

namespace Parser {
	Position LabelInfo::getPosition(void) const {
		return { intval >> 24, intval & 0xffffff };
	}

	void LabelInfo::setFromPosition(const Position &p) {
		intval = (p.page<<24) + p.line;
	}

	Instruction convertInstruction (string word) {
		to_lower(word); // Function calls are case insensitive.

		if (word == "move")                              return INSTR_MOVE;
		else if (word == "rotate" || word == "rot")      return INSTR_ROT;
		else if (word == "nop")                          return INSTR_NOP;
		else if (word == "goto")                         return INSTR_GOTO;
		else if (word == "ifgoto" || word == "if")       return INSTR_IFGOTO;
		else if (word == "store" || word == "sto")       return INSTR_STO;
		else if (word == "transfer" || word == "trans")  return INSTR_TRANS;
		else if (word == "page")                         return INSTR_PAGE;
		else if (word == "getloc" || word == "loc")      return INSTR_LOC;
		else return INSTR_INVALID;
	}

	// Parses the given `functionName` and `arguments` to a Statement.
	Statement parseStatement (string functionName, string arguments, const int lineIndex) {
		int i;
		Statement statement;
		statement.lineNumber = lineIndex;
		statement.instr = convertInstruction(functionName);
		if (statement.instr == INSTR_INVALID) {
			char *message;
			asprintf(&message, "Invalid instruction '%s'.", functionName.c_str());
			throw_error(lineIndex, message);
		}

		vector<string> argsRaw;
		int depth, cursor = 0;
		while (true) {
			depth = 0;
			for (i = cursor; i < (int)arguments.size(); i++) {
				if (depth == 0 && arguments[i] == ',') break;
				else if (arguments[i] == '(') depth++;
				else if (arguments[i] == ')') depth--;
			}
			if (i == (int)arguments.size()) {
				argsRaw.push_back(trim(arguments.substr(cursor)));
				break;
			} else {
				argsRaw.push_back(trim(arguments.substr(cursor, i)));
				cursor = i + 1;
			}
		}
		statement.args.resize(argsRaw.size());

		for (i = 0; i < (int)argsRaw.size(); i++) {
			parseExpression(&statement.args[i], tokeniseExpression(argsRaw[i]));
		}

		return statement;
	}

	// Parses the given `lines` with the given `fname`.
	Program parse (const char *const fname, const vector<string> &lines) {
		int curPage = 0;
		bool seenPages[16] = {false};

		Program program;
		program.id = genid();
		program.name = fname;
		program.pages.resize(16);

		// printf("\n");
		for (int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++) {
			// printf("%d: %s\n", lineIndex, lines[lineIndex].c_str());

			string line = lines[lineIndex];
			string trimmed = trim(line);
			if (trimmed.size() == 0) continue;
			vector<string> words = split(trimmed, ' ', 1);

			if (words[0] == "#page") { // page
				int id = stoi(words[1]);

				if (!seenPages[id]) {
					seenPages[id] = true;
					curPage = id;
				} else {
					char *message;
					asprintf(&message, "Page with ID '%d' already declared.", id);
					throw_error(lineIndex, message);
				}
			} else if (words[0] == "#name") { // name
				program.name = words[1];
			} else if (words[0] == "#author") { // author
				program.author = words[1];
			} else if (words[0][0] == '#') { // meta attribute
				continue;
			} else if (trimmed[trimmed.size() - 1] == ':') { // label
				string labelName = trimmed.substr(0, trimmed.size() - 1);

				int id = genid();
				Position position = {curPage, lineIndex};
				LabelInfo labelInfo = {id, 0};
				labelInfo.setFromPosition(position);

				program.labels.emplace(labelName, labelInfo);
			} else { // function call
				program.pages[curPage].push_back(parseStatement(words[0], words[1], lineIndex));
			}
		}

		return program;
	}
}
