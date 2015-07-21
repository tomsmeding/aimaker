#pragma once

#include <cmath>
#include <unordered_map>
#include <vector>
#include "lang/expression.h"
#include "util.h"

using namespace std;

class Board;
class Bot {
public:
	const Parser::Program *program;

private:
	int curInstr;
	int curPage;
	bool isDead;
	int _workingFor;
	Board *board;
	unordered_map<string, Parser::Variable> memoryMap;
	vector<vector<Parser::Statement>> pages;

	void storeVariable(const string&, const Parser::Variable&, const int = -1);
	int calculateMemorySize(void) const;
	bool reachedMemoryLimit(void) const;
	pair<int, int> executeCurrentLine(void);
	void jumpTo(int, int);
	pair<int, int> calculateNextLocation(bool) const;

public:
	int x, y, dir;
	bool isAsleep;
	int id;
	int index;
	int tier;

	Bot(const Parser::Program*, Board*, pair<int, int>, int);
	Bot(Bot*, int, pair<int, int>, int);
	bool isWorking(void) const;
	int workingFor(void) const;
	pair<int, int> getPos(void) const;
	int getDir(void) const;
	bool nextTick(void);
	void copyPage(int, const vector<Parser::Statement>&);
};
