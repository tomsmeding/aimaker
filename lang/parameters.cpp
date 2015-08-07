#include <cmath>
#include <map>
#include "parser.h"
#include "parameters.h"

int instructionWorkTime (const Parser::Instruction instr, const int arg) {
	switch (instr) {
	case Parser::INSTR_GOTO:       return arg == 0 ? 1 : 3;
	case Parser::INSTR_IFGOTO:     return arg == 0 ? 2 : 4;
	case Parser::INSTR_LOC:        return 2;
	case Parser::INSTR_LOOK:       return 3;
	case Parser::INSTR_MOVE:       return arg == 0 ? 3 : 4;
	case Parser::INSTR_NOP:        return 1;
	case Parser::INSTR_PAGE:       return 2;
	case Parser::INSTR_ROT:        return 2;
	case Parser::INSTR_STO:        return 1;

	// arg is page size
	case Parser::INSTR_TRANS:      return floor(5 + log10(arg + 1));
	case Parser::INSTR_TRANSLOCAL: return floor(3 + log10(arg + 1));

	case Parser::INSTR_BUILD:      return floor(20 + pow(7, arg));
	case Parser::INSTR_WAKE:       return 4;
	case Parser::INSTR_SLEEP:      return 1;
	case Parser::INSTR_PRINT:      return 1;
	case Parser::INSTR_PRINT_VARS: return 1;
	case Parser::INSTR_BREAK:      return 1;
	case Parser::INSTR_MAKEARR:    return 1;
	case Parser::INSTR_AT:         return 1;
	case Parser::INSTR_PUSH:       return 1;
	case Parser::INSTR_DEL:        return 1;
	case Parser::INSTR_LENGTH:     return 1;

	case Parser::INSTR_INVALID:
	default:                       return 1; // eh?
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
	{ Parser::INSTR_STOP_MATCH,  0 },
	{ Parser::INSTR_PRINT,       0 },
	{ Parser::INSTR_PRINT_VARS,  0 },
	{ Parser::INSTR_BREAK,       0 },
	{ Parser::INSTR_AT,          0 },
	{ Parser::INSTR_MAKEARR,     0 },
	{ Parser::INSTR_PUSH,        0 },
	{ Parser::INSTR_DEL,         0 },
	{ Parser::INSTR_LENGTH,      0 },
	{ Parser::INSTR_INVALID,     0 }
};

Params params;
