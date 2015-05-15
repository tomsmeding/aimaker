#pragma once

#include <string>
#include <vector>
#include "bot.h"

using namespace std;

class Board {
private:
	int size;
	vector<int> board;
	vector<Bot> bots;
public:
	Board(int);
	int at(int, int);
	void nextTick(void);
	string render(void);
};
