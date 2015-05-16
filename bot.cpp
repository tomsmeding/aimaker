#include <utility>
#include "bot.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curInstr(0), curPage(0), tick(0), x(0), y(0), dir(0), board(_board), program(_program) {
	id = genid();
}

void Bot::jumpTo(int page, int instr) {
	if(page < 0 || page >= (int)program->pages.size() ||
			instr < 0 || instr >= (int)program->pages[page].size()) {
		curPage = 0;
		curInstr = 0;
	} else {
		curPage = page;
		curInstr = instr;
	}
}

pair<int, int> Bot::getPos(void) {
	return make_pair(x, y);
}

int Bot::getDir(void) {
	return dir;
}

pair<int, int> Bot::executeCurrentLine() {
	const Parser::Statement currentStatement = pages[curPage][curInstr];

	switch (currentStatement.instr) {
		case Parser::INSTR_MOVE: {
			Parser::Argument argument = currentStatement.args[0];
			if (argument.type == Parser::Argument::Type::ARGT_NUMBER) {
				bool forwards = (bool) argument.intVal;
				int deltaX = 0;
				int deltaY = 0;

				     if (dir == 0) deltaX =  1;
				else if (dir == 2) deltaX = -1;
				else if (dir == 1) deltaY =  1;
				else if (dir == 3) deltaY = -1;

				if (!forwards) {
					deltaX = deltaX * -1;
					deltaX = deltaY * -1;
				}

				int x = this->x + deltaX;
				int y = this->y + deltaY;

				if (board->canMoveTo(deltaX, deltaY)) {
					this->x = x;
					this->y = y;
				} else {
					// We can't move there.
				}

			} else {
				// Wrong argument type.
			}
			break;
		}

		case Parser::INSTR_INVALID:
		case Parser::INSTR_NOP:
			break;
	}

	return make_pair(curPage, curInstr + 1);
}

void Bot::nextTick(void) {
	pair<int, int> pair = executeCurrentLine();

	curPage = pair.first;
	curInstr = pair.second;

	tick++;
}
