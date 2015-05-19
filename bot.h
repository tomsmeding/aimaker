#pragma once

#include <cmath>
#include <unordered_map>
#include <vector>
#include "lang/parser.h"
#include "lang/parser.h"
#include "util.h"

using namespace std;

class Board;
class Bot {
private:
	int curInstr;
	int curPage;
	int _workingFor;
	int x, y, dir;
	Board *board;

	pair<int, int> executeCurrentLine(void);
	void jumpTo(int, int);
	pair<int, int> calculateNextLocation(bool) const;

public:
	bool isAsleep;
	const int id;
	const Parser::Program *program;
	unordered_map<string, int> memoryMap;
	vector<vector<Parser::Statement>> pages;

	Bot(const Parser::Program*, Board*, pair<int, int>);
	bool isWorking(void) const;
	int workingFor(void) const;
	pair<int, int> getPos(void) const;
	int getDir(void) const;
	void nextTick(void);
	void copyPage(int, vector<Parser::Statement>);
};
