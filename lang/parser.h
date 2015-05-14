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

	class Argument {

	};

	class Statement {
	public:
		Instruction instr;
		vector<Argument> args;
	};

	typedef vector<Statement> Codepage;
	typedef pair<int, int> Position;

	struct ParseResult {
		vector<Codepage> pages;
		unordered_map<string, Position> labels;
	};

	Instruction convinstr(string word);

	ParseResult parser(string);

}
