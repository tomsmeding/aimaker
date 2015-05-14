#include "bot.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curPage(0), curInstr(0), program(_program), board(_board) {
	id = genid();
}

void Bot::jumpTo(int page, int statement) {

}
