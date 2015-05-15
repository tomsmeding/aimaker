#include <string>
#include <cstdio>
#include "parser.h"
#include "../util.h"

namespace Parser {
	Instruction convertInstruction(string word) {
		     if (word == "move") return INSTR_MOVE;
		else if (word == "sto")  return INSTR_STO;
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
					asprintf(&message, "Error at line %d: Page with ID '%d' already declared.", lineIndex, id);
					throw message;
				}
			} else if (words[0] == "#name") { // name
				program.name = words[1];
			} else if (words[0][0] == '#') { // meta attribute
				continue;
			} else if (trimmed[trimmed.size() - 1] == ':') { // label
				string labelName = trimmed.substr(0, trimmed.size() - 1);
				pair<int, int> position = make_pair(curPage, lineIndex);

				program.labels.emplace(make_pair(labelName, position));
			} else { // function call
				Statement statement;
				statement.instr = convertInstruction(words[0]);

				vector<string> argsRaw = split(words[1], ',');
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

				program.pages[curPage].push_back(statement);
			}
		}

		return program;
	}
}
