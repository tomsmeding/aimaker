#include <utility>
#include "bot.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curPage(0), curInstr(0), x(0), y(0), dir(0), board(_board), program(_program) {
	id = genid();
}

void Bot::jumpTo(int page, int statement) {

}

pair<int, int> Bot::getPos(void){
	return make_pair(x, y);
}

int Bot::getDir(void){
	return dir;
}

void Bot::nextTick(void){
	;
}
