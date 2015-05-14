#pragma once

#include <unordered_map>
#include <vector>
#include "lang/parser.h"
#include "util.h"

using namespace std;

class Bot {
private:
	int curPage;
	int curInstr;
	void jumpTo(int, int);

public:
	int id;
	unordered_map<string, int> memoryMap;
	vector<vector<Parser::Statement>> pages;
	Bot();
};
