#include <utility>
#include "bot.h"
#include "board.h"
#include "lang/parameters.h"

using namespace std;

Bot::Bot(const Parser::Program *_program, Board *_board, pair<int, int> startingPos) : curInstr(0), curPage(0), _workingFor(0), dir(0), board(_board), id(genid()), program(_program) {
	x = startingPos.first;
	xy = startingPos.second;
}

void Bot::jumpTo(int page, int instr) {
	if (page < 0 || page >= (int)pages.size() ||
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
	bool didJump = false;

	_workingFor = instructionWorkTime(currentStatement.instr);

	switch (currentStatement.instr) {
	case Parser::INSTR_MOVE: {
		const Parser::Argument argument = currentStatement.args[0];
		if (argument.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		const bool forwards = (bool) Parser::evaluateExpression(argument, memoryMap);

		const pair<int, int> newLocation = calculateNextLocation(forwards);
		const int x = newLocation.first;
		const int y = newLocation.second;

		if (board->canMoveTo(x, y)) {
			this->x = x;
			this->y = y;
		} else {
			// We can't move there.
		}
		break;
	}

	case Parser::INSTR_ROT: {
		const Parser::Argument argument = currentStatement.args[0];
		if (argument.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		dir += Parser::evaluateExpression(argument, memoryMap) % 4;
		break;
	}

	case Parser::INSTR_GOTO: {
		const Parser::Argument target = currentStatement.args[0];
		if (target.type != Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		unordered_map<string, Parser::Position>::const_iterator labelit = program->labels.find(target.strval);
		if (labelit == program->labels.end()) {
			// Label not found.
			break;
		}
		jumpTo(labelit->second.page, labelit->second.line);
		didJump = true;
		break;
	}

	case Parser::INSTR_IFGOTO: {
		const Parser::Argument condition = currentStatement.args[0];
		const Parser::Argument target = currentStatement.args[1];
		if (condition.type == Parser::EN_LABEL || target.type != Parser::EN_LABEL) {
			// Wrong argument type(s).
			break;
		}
		if (!Parser::evaluateExpression(condition, memoryMap)) break; // that's the `if`
		unordered_map<string, Parser::Position>::const_iterator labelit = program->labels.find(target.strval);
		if (labelit == program->labels.end()) {
			// Label not found.
			break;
		}
		jumpTo(labelit->second.page, labelit->second.line); // that's the `goto`
		didJump = true;
		break;
	}

	case Parser::INSTR_STO: {
		Parser::Argument varNameArgument = currentStatement.args[0];
		Parser::Argument valueArgument = currentStatement.args[1];

		if (varNameArgument.type != Parser::EN_VARIABLE || valueArgument.type == Parser::EN_LABEL) {
			// Wrong argument type(s).
			break;
		}
		const int value = Parser::evaluateExpression(valueArgument, memoryMap);
		memoryMap[varNameArgument.strval] = value;
		break;
	}

	case Parser::INSTR_TRANS: {
		Parser::Argument pageIdArgument = currentStatement.args[0];
		Parser::Argument targetIdArgument = currentStatement.args[1];

		if (pageIdArgument.type == Parser::EN_LABEL || targetIdArgument.type == Parser::EN_LABEL) {
			// Wrong argument type(s).
			break;
		}
		vector<Parser::Statement> page = pages[Parser::evaluateExpression(pageIdArgument, memoryMap)];

		const pair<int, int> targetLocation = calculateNextLocation(true);
		Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

		if (targetBot != NULL) {
			targetBot->copyPage(Parser::evaluateExpression(targetIdArgument, memoryMap), page);
		}

		_workingFor = instructionWorkTime(currentStatement.instr, page.size());
	}

	case Parser::INSTR_PAGE: {
		Parser::Argument target = currentStatement.args[0];

		if (target.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		jumpTo(Parser::evaluateExpression(target, memoryMap), 0);
		break;
	}

	case Parser::INSTR_NOP:
	case Parser::INSTR_INVALID:
		break;
	}

	return make_pair(curPage, curInstr + !didJump);
}

void Bot::nextTick(void) {
	if (_workingFor > 0) {
		_workingFor--;
		return;
	} else {
		pair<int, int> pair = executeCurrentLine();

		curPage = pair.first;
		curInstr = pair.second;
	}
}
