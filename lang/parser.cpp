#include <string>
#include <cstdio>
#include "parser.h"
#include "../util.h"

namespace Parser {
	Instruction convertInstruction(string word) {
		     if (word == "move") return INSTR_MOVE;
		else if (word == "sto")  return INSTR_STO;
	}

	// Parses the given `functionName` and `arugments` to a Statement.
	Statement parseStatment(string functionName, string arguments) {
		Statement statement;
		statement.instr = convertInstruction(functionName);

		vector<string> argsRaw = split(arguments, ',');
		statement.args.resize(argsRaw.size());

		for (int i = 0; i < (int)argsRaw.size(); i++) {
			if (is_numberic(argsRaw[i])) {
				statement.args[i].type = Argument::Type::ARGT_NUMBER;
				statement.args[i].intVal = stoi(argsRaw[i]);
			} else {
				statement.args[i].type = Argument::Type::ARGT_VARIABLE;
				statement.args[i].stringVal = argsRaw[i];
			}
		}

		return statement;
	}

	// Parses the given `lines` with the given `fname`.
	Program parse(const char *const fname, const vector<string> &lines) {
		int curPage = 0;
		bool seenPages[16];

		Program program;
		program.id = genid();
		program.name = fname;
		program.pages.resize(16);

		for (int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++) {
			string line = lines[lineIndex];
			string trimmed = trim(line);
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
				pair<int, int> position = make_pair(curPage, lineIndex);

				program.labels.emplace(make_pair(labelName, position));
			} else { // function call
				program.pages[curPage].push_back(parseStatment(words[0], words[1]));
			}
		}

		return program;
	}
}
