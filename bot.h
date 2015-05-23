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
	int _workingFor;
	int x, y, dir;
	Board *board;
	unordered_map<string, Parser::Variable> memoryMap;
	vector<vector<Parser::Statement>> pages;

	void storeVariable(const string&, const int, const int = -1);
	int calculateMemorySize(void) const;
	bool reachedMemoryLimit(void) const;
	pair<int, int> executeCurrentLine(void);
	void jumpTo(int, int);
	pair<int, int> calculateNextLocation(bool) const;

public:
	bool isAsleep;
	const int id;

	Bot(const Parser::Program*, Board*, pair<int, int>);
	bool isWorking(void) const;
	int workingFor(void) const;
	pair<int, int> getPos(void) const;
	int getDir(void) const;
	void nextTick(void);
	void copyPage(int, vector<Parser::Statement>);
};
