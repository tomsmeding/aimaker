#include <sstream>
#include "board.h"

using namespace std;

Board::Board(int size): size(size), tick(0) {
	id = genid();
}

int Board::currentTick(void) const {
	return tick;
}

void Board::nextTick(void) {
	for (int i = 0; i < (int)bots.size(); i++) {
		Bot &bot = bots[i];
		bool died = bot.nextTick();

		if (died) {
			bots.erase(bots.begin() + i);
			cerr << "bot " << bot.index << " erased" << endl;
			i--;
		}
	}

	tick++;
}

Bot *Board::at(int x, int y) {
	for (Bot &bot : bots) {
		pair<int, int> location = bot.getPos();

		if (location.first == x && location.second == y)
			return &bot;
	}

	return NULL;
}

string Board::render(void) const {
	const int linelen = 4*size+2;
	stringstream ss;
	int x, y;
	ss << '+';
	for (x = 0; x < size; x++)ss << "---+";
	ss << endl;
	for (y = 0; y < size; y++) {
		ss << '|';
		for (x = 0; x < size; x++)ss << "   |";
		ss << endl << '|';
		for (x = 0; x < size; x++)ss << "   |";
		ss << endl;
		ss << '+';
		for (x = 0; x < size; x++)ss << "---+";
		ss << endl;
	}
	string s(move(ss.str()));
	int idx, dir;
	pair<int, int> pos;
	const char dirchars[4][6] = {
		{'/' , '^' , '\\', ' ' , ' ' , ' ' },
		{' ' , ' ' , '\\', ' ' , ' ' , '/' },
		{' ' , ' ' , ' ' , '\\', '_' , '/' },
		{'/' , ' ' , ' ' , '\\', ' ' , ' ' }
	};
	for (Bot b : bots) {
		pos = b.getPos();
		dir = mod(b.getDir(), 4);
		idx = linelen + 3 * linelen * pos.second + 4 * pos.first + 1;
		s[idx + 0] = dirchars[dir][0];
		s[idx + 1] = dirchars[dir][1];
		s[idx + 2] = dirchars[dir][2];
		s[idx + linelen + 0] = dirchars[dir][3];
		s[idx + linelen + 1] = dirchars[dir][4];
		s[idx + linelen + 2] = dirchars[dir][5];
		s[idx+1]="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[b.index%62];
		s[idx+linelen+1]=" 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[b.index/62];
	}
	return s;
}

bool Board::canMoveTo(int x, int y) const {
	for (const Bot &bot : bots) {
		pair<int, int> location = bot.getPos();

		if (location.first == x && location.second == y) return false;
	}
	return (x < 0 || x >= size) || (y < 0 || y >= size);
}
