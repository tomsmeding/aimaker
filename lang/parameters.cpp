#include <cmath>
#include <map>
#include "parser.h"
#include "parameters.h"

int instructionWorkTime (const Parser::Instruction instr, const int arg) {
	switch (instr) {
	case Parser::INSTR_GOTO:    return 1;
	case Parser::INSTR_IFGOTO:  return 2;
	case Parser::INSTR_LOC:     return 2;
	case Parser::INSTR_LOOK:    return 3;
	case Parser::INSTR_MOVE:    return 5;
	case Parser::INSTR_NOP:     return 1;
	case Parser::INSTR_PAGE:    return 2;
	case Parser::INSTR_ROT:     return 5;
	case Parser::INSTR_STO:     return 1;

	// arg is page size
	case Parser::INSTR_TRANS:   return floor(5 + log10(arg + 1));
	case Parser::INSTR_TRANSLOCAL: return floor(3 + log10(arg + 1));

	case Parser::INSTR_BUILD:   return 7+4*arg;
	case Parser::INSTR_WAKE:    return 3;
	case Parser::INSTR_SLEEP:   return 1;

	case Parser::INSTR_INVALID:
	default:                    return 1; // eh?
	}
};

map<Parser::Instruction, int> instr_tier_map = {
	{ Parser::INSTR_GOTO,        0 },
	{ Parser::INSTR_IFGOTO,      0 },
	{ Parser::INSTR_LOC,         0 },
	{ Parser::INSTR_DIR,         0 },
	{ Parser::INSTR_LOOK,        0 },
	{ Parser::INSTR_MOVE,        0 },
	{ Parser::INSTR_NOP,         0 },
	{ Parser::INSTR_PAGE,        0 },
	{ Parser::INSTR_ROT,         0 },
	{ Parser::INSTR_STO,         0 },
	{ Parser::INSTR_SUICIDE,     0 },
	{ Parser::INSTR_TRANS,       1 },
	{ Parser::INSTR_TRANSLOCAL,  1 },
	{ Parser::INSTR_BUILD,       2 },
	{ Parser::INSTR_WAKE,        1 },
	{ Parser::INSTR_SLEEP,       0 },
	{ Parser::INSTR_INVALID,     0 }
};

Params params;
