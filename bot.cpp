#include <utility>
#include <cassert>
#include "bot.h"
#include "board.h"
#include "lang/parameters.h"

using namespace std;

extern Params params;

Bot::Bot(const Parser::Program *_program, Board *_board, pair<int, int> startingPos, int _index) :
	program(_program),
	curInstr(0),
	curPage(0),
	isDead(false),
	_workingFor(0),
	board(_board),

	pages(program->pages),

	x(startingPos.first), y(startingPos.second),
	dir(0),
	isAsleep(false),
	id(genid()),
	index(_index) {}

Bot::Bot(Bot *parent, pair<int, int> startingPos, int _index) :
	program(parent->program),
	curInstr(0),
	curPage(0),
	isDead(false),
	_workingFor(0),
	board(parent->board),

	pages(parent->pages),

	x(startingPos.first), y(startingPos.second),
	dir(parent->dir),
	isAsleep(true),
	id(genid()),
	index(_index) {}

void Bot::jumpTo(int page, int instr) {
	if (page < 0 || page >= (int)pages.size() ||
			instr < 0 || instr >= (int)pages[page].size()) {
		cerr<<"Bot with index "<<this->index<<" just jumped to 0.0"<<endl;
		curPage = 0;
		curInstr = 0;
	} else {
		cerr<<"Bot with index "<<this->index<<" just jumped to "<<page<<'.'<<instr<<endl;
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
	int deltaX = mod(dir, 2) == 1 ? 2 - mod(dir, 4) : 0;
	int deltaY = mod(dir, 2) == 0 ? mod(dir, 4) - 1 : 0;

	if (!forwards) {
		deltaX = deltaX * -1;
		deltaY = deltaY * -1;
	}

	cerr<<"calculateNextLocation: dx="<<deltaX<<" dy="<<deltaY<<endl;

	return make_pair(x + deltaX, y + deltaY);
}

void Bot::copyPage(int targetId, const vector<Parser::Statement> &page) {
	pages[targetId] = page;

	cerr << this->index << endl;

	for (Parser::Statement &instr : pages[targetId]) {
		// Reset the linenumbers, they don't make sense anymore.
		instr.lineNumber = -1;
	}

	if (curPage == targetId && curInstr >= (int)pages[curPage].size()) {
		curInstr = 0;
	}
}

pair<int, int> Bot::executeCurrentLine() {
	/*cerr << "------------------------" << endl;
	cout << curPage << "." << curInstr << endl;
	auto statements = pages.at(curPage);
	for (auto statement : statements) {
		cout << "found statement in pages[" << curPage << "] with type: " << statement.instr << endl;
	}
	const Parser::Statement currentStatement = pages.at(curPage).at(curInstr);
	cout << "page: " << curPage << " | instruction: " << curInstr << ", with type " << currentStatement.instr << endl;
	cout << "pages size: " << pages.size() << endl;
	cout << "pages[" << curPage << "] size: " << pages.at(curPage).size() << endl;*/
	const Parser::Statement currentStatement = pages.at(curPage).at(curInstr);
	bool didJump = false;
	int workTimeArg = 0;
	const int lineNumber = currentStatement.lineNumber;
	memoryMap["_rip"] = Parser::ptoi({ curPage, curInstr });

	// cerr << "Working for: " << _workingFor << endl;

	switch (currentStatement.instr) {
	case Parser::INSTR_MOVE: {
		const Parser::Argument argument = currentStatement.args[0];
		if (argument.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		const bool forwards = (bool) Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels).getInt(lineNumber);

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
		dir += mod(Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels).getInt(lineNumber), 4);
		break;
	}

	case Parser::INSTR_GOTO: {
		const Parser::Argument target = currentStatement.args[0];

		int intpos = Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels).getInt(lineNumber);
		Parser::Position pos = Parser::itop(intpos);

		memoryMap["_prevloc"] = Parser::ptoi({ curPage, curInstr + 1 });
		jumpTo(pos.page, pos.line);
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
		if (!Parser::evaluateExpression(condition, lineNumber, memoryMap, program->labels).getInt(lineNumber)) break; // that's the `if`
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

		const int value = Parser::evaluateExpression(valueArgument, lineNumber, memoryMap, program->labels).getInt(lineNumber);
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
		vector<Parser::Statement> page = pages[Parser::evaluateExpression(pageIdArgument, lineNumber, memoryMap, program->labels).getInt(lineNumber)];

		const pair<int, int> targetLocation = calculateNextLocation(true);
		Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

		if (targetBot != NULL) {
			cerr << "copying to bot with id " << targetBot->index << endl;

			targetBot->copyPage(Parser::evaluateExpression(targetIdArgument, lineNumber, memoryMap, program->labels).getInt(lineNumber), page);

			cout << "page copied" << endl;
			cout << "copied page from one bot to the other, the instruction type of the first instruction of the copied page in the bot is " << (int) targetBot->pages[targetIdArgument.intval][0].instr << endl;
		}

		workTimeArg = page.size();

		break;
	}

	case Parser::INSTR_PAGE: {
		Parser::Argument target = currentStatement.args[0];

		if (target.type == Parser::EN_LABEL) {
			// Wrong argument type.
			break;
		}
		jumpTo(Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels).getInt(lineNumber), 0);
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

	case Parser::INSTR_SUICIDE: {
		cout << "boom said bot with index " << index << endl;
		isDead = true;
		break;
	}

	case Parser::INSTR_LOOK: {
		//bit 1: is_wall
		//bit 2: is_bot
		//bit 4: is_my_bot
		//bit 8: is_bot_sleeping

		Parser::Argument varNameArgument = currentStatement.args[0];

		if (varNameArgument.type != Parser::EN_VARIABLE) {
			// Wrong argument type(s).
			break;
		}

		const pair<int, int> targetLocation = calculateNextLocation(true);
		Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

		int response = 0;
		if(!this->board->insideBounds(targetLocation.first, targetLocation.second)){
			response |= 1;
		} else {
			if(targetBot){
				response |= 2;
				if(targetBot->program->id == this->program->id) response |= 4;
				if(targetBot->isAsleep) response |= 8;
			}
		}
		storeVariable(varNameArgument.strval, response);
		cerr<<"Put "<<response<<" in "<<varNameArgument.strval<<" for bot "<<this->index<<endl;

		break;
	}

	case Parser::INSTR_TRANSLOCAL: {
		Parser::Argument pageIdArgument = currentStatement.args[0];
		Parser::Argument targetIdArgument = currentStatement.args[1];

		if (pageIdArgument.type == Parser::EN_LABEL || targetIdArgument.type == Parser::EN_LABEL) {
			// Wrong argument type(s).
			break;
		}

		vector<Parser::Statement> page = pages[Parser::evaluateExpression(pageIdArgument, lineNumber, memoryMap, program->labels).getInt(lineNumber)];
		copyPage(Parser::evaluateExpression(targetIdArgument, lineNumber, memoryMap, program->labels).getInt(lineNumber), page);
		workTimeArg = page.size();

		break;
	}

	case Parser::INSTR_BUILD: {
		const pair<int, int> targetLocation = calculateNextLocation(true);

		if (board->canMoveTo(targetLocation.first, targetLocation.second)) {
			Bot bot(this, targetLocation, board->bots.size());
			board->addBot(bot);
		}

		break;
	}

	case Parser::INSTR_WAKE: {
		const pair<int, int> targetLocation = calculateNextLocation(true);
		Bot *targetBot = board->at(targetLocation.first, targetLocation.second);
		if (targetBot != NULL) targetBot->isAsleep = false;
		break;
	}

	case Parser::INSTR_SLEEP: {
		isAsleep = true;
		break;
	}

	case Parser::INSTR_NOP:
	case Parser::INSTR_INVALID:
		break;
	}

	_workingFor = instructionWorkTime(currentStatement.instr, workTimeArg);

	return make_pair(curPage, curInstr + !didJump);
}

bool Bot::nextTick(void) {
	if (isAsleep) {
		return false;
	}
	if (_workingFor > 0) {
		_workingFor--;
		return false;
	} else {
		pair<int, int> pair = executeCurrentLine();

		_workingFor--;

		curPage = pair.first;
		curInstr = pair.second;
		if(curPage >= (int)pages.size()){
			assert(false);
		}
		if(curInstr >= (int)pages[curPage].size()){
			isAsleep = true;
			curInstr = 0;
		}
	}

	cout << "nextTicked on bot with index " << index << " at " << curPage << "." << curInstr << ", dead: " << (int) isDead << "; next instr to exec: " << pages[curPage][curInstr].instr << endl;
	return isDead;
}
