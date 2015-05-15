#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <string>

using namespace std;

namespace Parser {

	enum Instruction {
		INSTR_MOVE,
		INSTR_STO
	};

	struct Argument {
		enum Type {
			ARGT_NUMBER,
			ARGT_VARIABLE
		};
		Type type;
		union {
			int intVal;
			string stringVal;
		};
	};

	struct Statement {
		Instruction instr;
		vector<Argument> args;
	};

	typedef vector<Statement> Codepage;
	typedef pair<int, int> Position;

	struct Program {
		int id;
		vector<Codepage> pages;
		unordered_map<string, Position> labels;
	};

	Instruction convertInstruction(string word);

	Program parser(const vector<string>);

}
