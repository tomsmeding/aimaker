#pragma once

#include <unordered_map>
#include <vector>
#include "lang/parser.h"
#include "util.h"
#include "board.h"
#include "lang/parser.h"

using namespace std;

class Bot {
private:
	int curPage;
	int curInstr;
	int x, y, dir;
	void jumpTo(int, int);
	const Parser::Program *program;
	Board *board;

public:
	int id;
	unordered_map<string, int> memoryMap;
	vector<vector<Parser::Statement>> pages;
	Bot(const Parser::Program*, Board*);
	pair<int, int> getPos(void);
	int getDir(void);
};
