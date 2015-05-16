#include <utility>
#include "bot.h"
#include "board.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curInstr(0), curPage(0), tick(0), x(0), y(0), dir(0), board(_board), id(genid()), program(_program) {}

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

pair<int, int> Bot::getPos(void) const {
	return make_pair(x, y);
}

int Bot::getDir(void) const {
	return dir;
}

pair<int, int> Bot::executeCurrentLine() {
	const Parser::Statement currentStatement = pages[curPage][curInstr];

	switch (currentStatement.instr) {
		case Parser::INSTR_MOVE: {
			Parser::Argument argument = currentStatement.args[0];
			if (argument.type == Parser::Argument::Type::ARGT_NUMBER) {
				const bool forwards = (bool) argument.intVal;
				int deltaX = dir % 2 == 1 ? 2 - dir : 0;
				int deltaY = dir % 2 == 0 ? dir - 1 : 0;

				if (!forwards) {
					deltaX = deltaX * -1;
					deltaX = deltaY * -1;
				}

				const int x = this->x + deltaX;
				const int y = this->y + deltaY;

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
