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
		INSTR_MOVE,
		INSTR_NOP,
		INSTR_PAGE,
		INSTR_ROT,
		INSTR_STO,
		INSTR_TRANS,
		INSTR_SUICIDE,

		INSTR_INVALID=255
	};

	typedef ExprNode Argument;

	struct Statement {
		Instruction instr;
		vector<Argument> args;
		int lineNumber;
	};

	typedef vector<Statement> Codepage;
	struct Position {
		int page, line;
	};
	struct LabelInfo {
		int id;
		int intval;
		Position getPosition(void) const;
		void setFromPosition(const Position&);
	};

	typedef unordered_map<string, LabelInfo> LabelMap;
	struct Program {
		int id;
		string name;
		string author;
		vector<Codepage> pages;
		LabelMap labels;
	};

	Instruction convertInstruction(string word);

	Program parse(const char *const, const vector<string>&);

}
