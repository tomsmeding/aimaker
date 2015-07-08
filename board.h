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
	vector<Bot> botqueue;

public:
	vector<Bot> bots;
	int id;

	Board(int);
	Bot* at(int, int);
	int currentTick(void) const;
	void nextTick(void);
	string render(void) const;
	bool canMoveTo(int, int) const;
	bool insideBounds(int, int) const;
	void addBot(Bot);
};
