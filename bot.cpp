#include <utility>
#include "bot.h"
#include "board.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board) : curInstr(0), curPage(0), _workingFor(0), x(0), y(0), dir(0), board(_board), id(genid()), program(_program) {}

void Bot::jumpTo(int page, int instr) {
	if(page < 0 || page >= (int)pages.size() ||
			instr < 0 || instr >= (int)pages[page].size()) {
		curPage = 0;
		curInstr = 0;
	} else {
		curPage = page;
		curInstr = instr;
	}
}

bool Bot::isWorking(void) const {
	return (bool) _workingFor;
}

int Bot::workingFor(void) const {
	return _workingFor;
}

pair<int, int> Bot::getPos(void) const {
	return make_pair(x, y);
}

int Bot::getDir(void) const {
	return dir;
}

pair<int, int> Bot::calculateNextLocation(bool forwards) const {
	int deltaX = dir % 2 == 1 ? 2 - dir : 0;
	int deltaY = dir % 2 == 0 ? dir - 1 : 0;

	if (!forwards) {
		deltaX = deltaX * -1;
		deltaX = deltaY * -1;
	}

	return make_pair(x + deltaX, y + deltaY);
}

void Bot::copyPage(int targetId, vector<Parser::Statement> page) {
	pages[targetId] = page;

	if (curInstr >= (int)pages[curPage].size()) {
		curInstr = 0;
	}
}

pair<int, int> Bot::executeCurrentLine() {
	const Parser::Statement currentStatement = pages[curPage][curInstr];

	switch (currentStatement.instr) {
		case Parser::INSTR_MOVE: {
			Parser::Argument argument = currentStatement.args[0];
			if (argument.type == Parser::Argument::ARGT_NUMBER) {
				const bool forwards = (bool) argument.intVal;

				const pair<int, int> newLocation = calculateNextLocation(forwards);
				const int x = newLocation.first;
				const int y = newLocation.second;

				if (board->canMoveTo(x, y)) {
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

		case Parser::INSTR_ROT:
		{
			Parser::Argument argument = currentStatement.args[0];
			if (argument.type == Parser::Argument::ARGT_NUMBER) {
				dir += argument.intVal % 4;
			} else {
				// Wrong argument type.
			}
			break;
		}

		case Parser::INSTR_STO:
		{
			Parser::Argument varNameArgument = currentStatement.args[0];
			Parser::Argument valueArgument = currentStatement.args[1];

			if (varNameArgument.type == Parser::Argument::ARGT_VARIABLE && valueArgument.type == Parser::Argument::ARGT_NUMBER) {
				memoryMap.emplace(make_pair(varNameArgument.stringVal, valueArgument.intVal));
			} else {
				// Wrong argument type.
			}
		}

		case Parser::INSTR_TRANS:
		{
			Parser::Argument pageIdArgument = currentStatement.args[0];
			Parser::Argument targetIdArgument = currentStatement.args[1];

			if (pageIdArgument.type == Parser::Argument::ARGT_NUMBER && targetIdArgument.type == Parser::Argument::ARGT_NUMBER) {
				vector<Parser::Statement> page = pages[pageIdArgument.ARGT_NUMBER];

				const pair<int, int> targetLocation = calculateNextLocation(true);
				Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

				if (targetBot != NULL) {
					targetBot->copyPage(targetIdArgument.intVal, page);
				}

				_workingFor=floor(5 + log(page.size()));
			} else {
				// Wrong argument type.
			}
		}

		case Parser::INSTR_INVALID:
		case Parser::INSTR_NOP:
			break;
	}

	return make_pair(curPage, curInstr + 1);
}

void Bot::nextTick(void) {
	if (_workingFor) {
		_workingFor--;
		return;
	} else {
		pair<int, int> pair = executeCurrentLine();

		curPage = pair.first;
		curInstr = pair.second;
	}
}
