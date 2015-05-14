#include <vector>
#include "bot.h"

template <int S>
class Board {
private:
	int board[S*S];
	vector<Bot> bots;
public:
	int at(int, int);
	void nextTick(void);
};
