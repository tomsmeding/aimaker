#include <utility>
#include "bot.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curPage(0), curInstr(0), program(_program), board(_board), x(0), y(0), dir(0) {
	id = genid();
}

void Bot::jumpTo(int page, int statement) {

}

pair<int, int> getPos(void){
	return make_pair(x, y);
}

int getDir(void){
	return dir;
}
