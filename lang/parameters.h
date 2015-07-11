#pragma once

#include "parser.h"

int instructionWorkTime(const Parser::Instruction, const int arg=0);

struct Params {
	int boardSize = 5;
	int maxBotMemory = 50;
	int maxPages = 16;
	bool parseOnly;
};
