#pragma once

#include "parser.h"

int instructionWorkTime(const Parser::Instruction, const int arg=0);

struct Params {
	int maxBotMemory = 50;
	int boardSize = 5;
	bool parseOnly;
	int maxPages = 16;
};
