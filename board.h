#pragma once

#include <string>
#include <vector>
#include "bot.h"

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
