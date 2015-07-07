#include <utility>
#include <iostream>
#include "bot.h"
#include "board.h"
#include "lang/parameters.h"

using namespace std;

extern Params params;

Bot::Bot(const Parser::Program *_program, Board *_board, pair<int, int> startingPos) :
	program(_program),
	curInstr(0),
	curPage(0),
	_workingFor(0),
	x(0), y(0), dir(0),
	board(_board),

	pages(program->pages),

	isAsleep(false),
	id(genid()) {

	x = startingPos.first;
	y = startingPos.second;
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

void Bot::storeVariable(const string &varName, const int value, const int lineIndex) {
	if (!reachedMemoryLimit()) {
		if (varName.find('_') == 0) {
			// Unmutable or disposal variable.
			return;
		}

		memoryMap[varName] = value;
	} else {
		char *message;
		asprintf(&message, "Bot memory reached ('%d')", params.maxBotMemory);
		throw_error(lineIndex, message);
	}
}

int Bot::calculateMemorySize(void) const {
	int sum = 0;

	for (const auto &var : memoryMap) {
		sum += var.second.getSize();
	}

	return sum;
}

bool Bot::reachedMemoryLimit(void) const {
	return calculateMemorySize() >= params.maxBotMemory;
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
	// cerr << "------------------------" << endl;
	// cerr << curPage << "." << curInstr << endl;
	// auto statements = pages.at(curPage);
	// for (auto statement : statements) {
	// 	cout << "found statement in pages[" << curPage << "] with type: " << statement.instr << endl;
	// }
	// cout << "page: " << curPage << " | instruction: " << curInstr << endl;
	// cout << "pages size: " << pages.size() << endl;
	// cout << "pages[" << curPage << "] size: " << pages.at(curPage).size() << endl;
	const Parser::Statement currentStatement = pages.at(curPage).at(curInstr);
	bool didJump = false;
	const int lineNumber = currentStatement.lineNumber;
	memoryMap["rip"] = curInstr;

	_workingFor = instructionWorkTime(currentStatement.instr);
	// cerr << "Working for: " << _workingFor << endl;

	switch (currentStatement.instr) {
	case Parser::INSTR_MOVE: {
		const Parser::Argument argument = currentStatement.args[0];
		if (argument.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		const bool forwards = (bool) Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels);

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
		dir += Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels) % 4;
		break;
	}

	case Parser::INSTR_GOTO: {
		const Parser::Argument target = currentStatement.args[0];

		unordered_map<string, Parser::LabelInfo>::const_iterator labelit = program->labels.find(target.strval);
		if (labelit == program->labels.end()) {
			// Label not found.
			char *message;
			asprintf(&message, "Label not found: '%s'", target.strval.c_str());
			throw_error(lineNumber, message);
		}

		memoryMap["_prevloc"] = labelit->second.id;
		jumpTo(labelit->second.getPosition().page, labelit->second.getPosition().line);
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
		if (!Parser::evaluateExpression(condition, lineNumber, memoryMap, program->labels)) break; // that's the `if`
		unordered_map<string, Parser::LabelInfo>::const_iterator labelit = program->labels.find(target.strval);
		if (labelit == program->labels.end()) {
			// Label not found.
			break;
		}
		jumpTo(labelit->second.getPosition().page, labelit->second.getPosition().line); // that's the `goto`
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

		const int value = Parser::evaluateExpression(valueArgument, lineNumber, memoryMap, program->labels);
		storeVariable(varNameArgument.strval, value, curInstr);
		break;
	}

	case Parser::INSTR_TRANS: {
		Parser::Argument pageIdArgument = currentStatement.args[0];
		Parser::Argument targetIdArgument = currentStatement.args[1];

		if (pageIdArgument.type == Parser::EN_LABEL || targetIdArgument.type == Parser::EN_LABEL) {
			// Wrong argument type(s).
			break;
		}
		vector<Parser::Statement> page = pages[Parser::evaluateExpression(pageIdArgument, lineNumber, memoryMap, program->labels)];

		const pair<int, int> targetLocation = calculateNextLocation(true);
		Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

		if (targetBot != NULL) {
			targetBot->copyPage(Parser::evaluateExpression(targetIdArgument, lineNumber, memoryMap, program->labels), page);
		}
	}

	case Parser::INSTR_PAGE: {
		Parser::Argument target = currentStatement.args[0];

		if (target.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		jumpTo(Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels), 0);
		didJump = true;
		break;
	}

	case Parser::INSTR_LOC: {
		Parser::Argument xTarget = currentStatement.args[0];
		Parser::Argument yTarget = currentStatement.args[1];

		if (xTarget.type == Parser::EN_VARIABLE &&
				yTarget.type == Parser::EN_VARIABLE) {
			storeVariable(xTarget.strval, this->x);
			storeVariable(yTarget.strval, this->y);
		} else {
			// Wrong argument type.
		}

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
