#include <cmath>
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
	case Parser::INSTR_BUILD:   return 7;
	case Parser::INSTR_TRANS:   return floor(5 + log10(arg + 1)); // arg is page size
	case Parser::INSTR_INVALID:
	default:                    return 1; // eh?
	}
};

Params params;
