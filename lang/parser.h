#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <string>
#include "expression.h"

using namespace std;

namespace Parser {

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

		INSTR_INVALID=255
	};

	typedef ExprNode Argument;

	struct Statement {
		Instruction instr;
		vector<Argument> args;
	};

	typedef vector<Statement> Codepage;
	struct Position {
		int page, line;
	};

	struct Program {
		int id;
		string name;
		string author;
		vector<Codepage> pages;
		unordered_map<string, Position> labels;
	};

	Instruction convertInstruction(string word);

	Program parse(const char *const,const vector<string>&);

}
