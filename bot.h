#pragma once

#include <unordered_map>
#include <vector>
#include "lang/parser.h"
#include "util.h"
#include "lang/parser.h"

using namespace std;

class Board;
class Bot {
private:
	int curInstr;
	int curPage;
	int tick;
	int x, y, dir;
	Board *board;

	void jumpTo(int, int);
	pair<int, int> executeCurrentLine(void);

public:
	const int id;
	const Parser::Program *program;
	unordered_map<string, int> memoryMap;
	vector<vector<Parser::Statement>> pages;

	Bot(const Parser::Program*, Board*);
	pair<int, int> getPos(void) const;
	int getDir(void) const;
	void nextTick(void);
};
