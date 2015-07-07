#pragma once

#include <string>
#include <vector>
#include "bot.h"

using namespace std;

class Board {
private:
	int size;
	int tick;
	vector<int> board;
public:
	vector<Bot*> bots;
	int id;

	Board(int);
	Bot* at(int, int);
	void nextTick(void);
	string render(void);
	bool canMoveTo(int, int);
};
