#pragma once

#include "parser.h"

int instructionWorkTime(const Parser::Instruction, const int arg=0);

struct Params {
	int maxBotMemory = 50;
	bool parseonly;
};
