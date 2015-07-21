#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <string>
#include "expression.h"

using namespace std;

namespace Parser {

	struct ExprNode;

	enum Instruction {
		INSTR_GOTO,
		INSTR_IFGOTO,
		INSTR_LOC,
		INSTR_LOOK,
		INSTR_MOVE,
		INSTR_NOP,
		INSTR_PAGE,
		INSTR_ROT,
		INSTR_STO,
		INSTR_SUICIDE,
		INSTR_TRANS,
		INSTR_TRANSLOCAL,
		INSTR_BUILD,
		INSTR_WAKE,
		INSTR_SLEEP,
		INSTR_STOP_MATCH,
		INSTR_DIR,
		INSTR_PRINT,

		INSTR_INVALID=255
	};

	typedef ExprNode Argument;

	struct Statement {
		Instruction instr;
		vector<Argument> args;
		int lineNumber;
	};

	struct Position {
		int page, line;
	};
	struct LabelInfo {
		int id;
		int intval;
		Position getPosition(void) const;
		void setFromPosition(const Position&);
	};

	typedef vector<Statement> Codepage;
	typedef unordered_map<string, LabelInfo> LabelMap;
	struct Program {
		int id;
		string name;
		string author;
		vector<Codepage> pages;
		LabelMap labels;
	};

	Instruction convertInstruction(string word);
	string convertInstructionReverse(Instruction instr);

	Program parse(const char *const, const vector<string>&);

	Position itop(int pos);
	int ptoi(const Position&);

}
