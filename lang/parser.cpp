#include <string>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "parser.h"
#include "parameters.h"
#include "../util.h"

extern Params params;
extern map<Parser::Instruction, int> instr_arity_map;

namespace Parser {

	Position itop(int pos) {
		return { pos >> 24, pos & 0xffffff };
	}

	int ptoi(const Position &pos) {
		return (pos.page<<24) + pos.line;
	}

	Position LabelInfo::getPosition(void) const {
		return itop(intval);
	}

	void LabelInfo::setFromPosition(const Position &p) {
		intval = ptoi(p);
	}

	bool isDebugInstr(Instruction instr) {
		switch (instr) {
			case INSTR_STOP_MATCH:
			case INSTR_BREAK:
				return true;
			default:
				return false;
		}
	}

	Instruction convertInstruction (string word) {
		to_lower(word); // Function calls are case insensitive.

		if      (word == "move" || word == "walk")                     return INSTR_MOVE;
		else if (word == "getloc" || word == "loc")                    return INSTR_LOC;
		else if (word == "getdir" || word == "dir")                    return INSTR_DIR;
		else if (word == "goto")                                       return INSTR_GOTO;
		else if (word == "ifgoto" || word == "if")                     return INSTR_IFGOTO;
		else if (word == "look")                                       return INSTR_LOOK;
		else if (word == "nop")                                        return INSTR_NOP;
		else if (word == "page")                                       return INSTR_PAGE;
		else if (word == "rotate" || word == "rot" || word == "turn")  return INSTR_ROT;
		else if (word == "store" || word == "sto")                     return INSTR_STO;
		else if (word == "suicide" || word == "fuckit")                return INSTR_SUICIDE;
		else if (word == "transfer" || word == "trans")                return INSTR_TRANS;
		else if (word == "build")                                      return INSTR_BUILD;
		else if (word == "wake" || word == "shake-awake")              return INSTR_WAKE;
		else if (word == "sleep")                                      return INSTR_SLEEP;
		else if (word == "stop-match")                                 return INSTR_STOP_MATCH;
		else if (word == "print")                                      return INSTR_PRINT;
		else if (word == "print-vars")                                 return INSTR_PRINT_VARS;
		else if (word == "break" || word == "breakpoint")              return INSTR_BREAK;
		else if (word == "getat" || word == "at")                      return INSTR_AT;
		else if (word == "makearr" || word == "arr")                   return INSTR_MAKEARR;
		else if (word == "push")                                       return INSTR_PUSH;
		else if (word == "del")                                        return INSTR_DEL;
		else if (word == "length" || word == "len")                    return INSTR_LENGTH;
		else return INSTR_INVALID;
	}

	string convertInstructionReverse (Instruction instr) {
		if (instr == INSTR_MOVE) return "MOVE";
		else if (instr == INSTR_LOC) return "LOC";
		else if (instr == INSTR_GOTO) return "GOTO";
		else if (instr == INSTR_IFGOTO) return "IFGOTO";
		else if (instr == INSTR_LOOK) return "LOOK";
		else if (instr == INSTR_NOP) return "NOP";
		else if (instr == INSTR_PAGE) return "PAGE";
		else if (instr == INSTR_ROT) return "ROT";
		else if (instr == INSTR_STO) return "STO";
		else if (instr == INSTR_SUICIDE) return "SUICIDE";
		else if (instr == INSTR_TRANS) return "TRANS";
		else if (instr == INSTR_BUILD) return "BUILD";
		else if (instr == INSTR_WAKE) return "WAKE";
		else if (instr == INSTR_SLEEP) return "SLEEP";
		else if (instr == INSTR_STOP_MATCH) return "STOP_MATCH";
		else if (instr == INSTR_PRINT) return "PRINT";
		else if (instr == INSTR_PRINT_VARS) return "PRINT_VARS";
		else if (instr == INSTR_BREAK) return "BREAK";
		else if (instr == INSTR_AT) return "AT";
		else if (instr == INSTR_MAKEARR) return "MAKEARR";
		else if (instr == INSTR_PUSH) return "PUSH";
		else if (instr == INSTR_DEL) return "DEL";
		else if (instr == INSTR_LENGTH) return "LENGTH";
		else if (instr == INSTR_NOP || instr == INSTR_INVALID) return "-INVALID-";
		else return "-UNKNOWN- ";
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
		} else if (isDebugInstr(statement.instr) && !params.allowDebug) {
			char *message;
			asprintf(&message, "Can only use instruction '%s' with debugging allowed (--allowdebug).", functionName.c_str());
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
				if (i != 0) argsRaw.push_back(trim(arguments.substr(cursor)));
				break;
			} else {
				argsRaw.push_back(trim(arguments.substr(cursor, i - cursor)));
				cursor = i + 1;
			}
		}
		statement.args.resize(argsRaw.size());

		for (i = 0; i < (int)argsRaw.size(); i++) {
			parseExpression(&statement.args[i], tokeniseExpression(argsRaw[i], lineIndex), lineIndex);
		}

		return statement;
	}

	void inlineLabels (ExprNode *node, const LabelMap &labels, const int lineNumber) {
		assert(node);
		if(node->type==EN_LABEL){
			if(node->hasval!=1){
				throw_error(lineNumber,"Label node at %p does not have a strval (internal error)",node);
			}
			if(node->left||node->right){
				throw_error(lineNumber,"Label node at %p has unexpected children (internal error)",node);
			}
			LabelMap::const_iterator it=labels.find(node->strval);
			if(it==labels.cend()){
				throw_error(lineNumber,"Label not found: '%s'",node->strval.c_str());
			}
			node->type=EN_NUMBER;
			node->hasval=2; //int
			node->strval.clear();
			node->intval=it->second.intval;
		} else {
			if(node->left)inlineLabels(node->left,labels,lineNumber);
			if(node->right)inlineLabels(node->right,labels,lineNumber);
		}
	}

	void inlineLabels (Program &program) {
		for(Codepage &page : program.pages){
			for(Statement &stmt : page){
				for(ExprNode &node : stmt.args){
					inlineLabels(&node,program.labels,stmt.lineNumber);
				}
			}
		}
	}

	// Parses the given `lines` with the given `fname`.
	Program parse (const char *const fname, const vector<string> &lines) {
		int curPage = 0, curInstr = 0;
		bool seenPages[params.maxPages];
		memset(seenPages, 0, params.maxPages * sizeof(bool));

		Program program;
		program.id = genid();
		program.name = fname;
		program.pages.resize(params.maxPages);

		// printf("\n");
		for (int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++) {
			// printf("%d: %s\n", lineIndex, lines[lineIndex].c_str());

			string line = lines[lineIndex];
			size_t pos = line.find("//");
			if (pos != string::npos) {
				line.erase(pos);
			}
			line = trim(line);
			if (line.size() == 0) continue;
			vector<string> words = split(line, ' ', 1);

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

				curInstr = 0;
			} else if (words[0] == "#name") { // name
				program.name = words[1];
			} else if (words[0] == "#author") { // author
				program.author = words[1];
			} else if (words[0][0] == '#') { // unknown meta attribute
				char *message;
				asprintf(&message, "Unrecognised meta-attribute '%s'.", words[0].c_str());
				throw_error(lineIndex, message);
			} else if (line[line.size() - 1] == ':') { // label
				string labelName = line.substr(0, line.size() - 1);

				int id = genid();
				Position position = {curPage, curInstr};
				LabelInfo labelInfo;
				labelInfo.id = id;
				labelInfo.setFromPosition(position);

				program.labels.emplace(labelName, labelInfo);
			} else { // function call
				const string args = words.size() == 1 ? "" : words[1];
				program.pages[curPage].push_back(parseStatement(words[0], args, lineIndex + 1));

				curInstr++;
			}
		}

		inlineLabels (program);

		return program;
	}
}
