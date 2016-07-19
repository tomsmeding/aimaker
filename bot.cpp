#include <cassert>
#include <cstdlib>
#include <utility>
#include "board.h"
#include "bot.h"
#include "lang/parameters.h"

using namespace std;

extern Params params;
extern map<Parser::Instruction, int> instr_tier_map;
extern map<Parser::Instruction, int> instr_arity_map;

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
	index(_index),
	tier(2) {}

Bot::Bot(Bot *parent, int _tier, pair<int, int> startingPos, int _index) :
	program(parent->program),
	curInstr(0),
	curPage(0),
	isDead(false),
	_workingFor(0),
	board(parent->board),

	//pages(parent->pages), //on purpose commented out

	x(startingPos.first), y(startingPos.second),
	dir(parent->dir),
	isAsleep(true),
	id(genid()),
	index(_index),
	tier(_tier) {
		pages.resize(params.maxPages);
	}

void Bot::jumpTo(int page, int instr) {
	if (page < 0 || page >= (int)pages.size() ||
			instr < 0 || instr >= (int)pages[page].size()) {
		//cerr<<"Bot with index "<<this->index<<" just jumped to 0.0"<<endl;
		curPage = 0;
		curInstr = 0;
	} else {
		//cerr<<"Bot with index "<<this->index<<" just jumped to "<<page<<'.'<<instr<<endl;
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

void Bot::storeVariable(const string &varName, const Parser::Variable &var, const int lineIndex) {
	if (!reachedMemoryLimit()) {
		if (varName.find('_') == 0) {
			// Unmutable or disposal variable.
			return;
		}

		memoryMap[varName] = var;
	} else {
		char *message;
		asprintf(&message, "Bot memory reached ('%d')", params.maxBotMemory);
		throw_error(lineIndex, message);
	}
}

Parser::Variable *Bot::getVariable(const string &varName) {
	auto vari = memoryMap.find(varName);
	if (vari == memoryMap.end()) {
		return nullptr;
	} else {
		return &memoryMap.at(varName);
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

	//cerr<<"calculateNextLocation: dx="<<deltaX<<" dy="<<deltaY<<endl;

	return make_pair(x + deltaX, y + deltaY);
}

void Bot::copyPage(int targetId, const vector<Parser::Statement> &page, bool sameProgram) {
	pages[targetId] = page;

	//cerr << this->index << endl;

	if (!sameProgram) {
		for (Parser::Statement &instr : pages[targetId]) {
			// Reset the linenumbers, they don't make sense anymore.
			instr.lineNumber = -1;
		}
	}

	if (curPage == targetId && curInstr >= (int)pages[curPage].size()) {
		curInstr = 0;
	}
}

pair<int, int> Bot::executeCurrentLine() {
	/*cerr << "------------------------" << endl;
	cerr << curPage << "." << curInstr << endl;
	auto statements = pages.at(curPage);
	for (auto statement : statements) {
		cerr << "found statement in pages[" << curPage << "] with type: " << statement.instr << endl;
	}
	const Parser::Statement currentStatement = pages.at(curPage).at(curInstr);
	cerr << "page: " << curPage << " | instruction: " << curInstr << ", with type " << currentStatement.instr << endl;
	cerr << "pages size: " << pages.size() << endl;
	cerr << "pages[" << curPage << "] size: " << pages.at(curPage).size() << endl;*/
	if (pages.at(curPage).size() == 0) return { curPage, curInstr }; // and caller will put bot to sleep
	const Parser::Statement currentStatement = pages.at(curPage).at(curInstr);

	auto tieri = instr_tier_map.find(currentStatement.instr);
	if (tieri == instr_tier_map.end()) throw_error(currentStatement.lineNumber, ("No tier found for instruction: " + convertInstructionReverse(currentStatement.instr)).c_str());
	bool canExecute = tier >= instr_tier_map.at(currentStatement.instr);

	bool didJump = false;
	int workTimeArg = 0;
	const int lineNumber = currentStatement.lineNumber;
	memoryMap["_rip"] = Parser::ptoi({ curPage, curInstr });

	// cerr << "Working for: " << _workingFor << endl;

	if (canExecute) {
		if ((int)currentStatement.args.size() != instr_arity_map[currentStatement.instr]) {
			throw_error(lineNumber, string(
				"Instruction " +
				Parser::convertInstructionReverse(currentStatement.instr) +
				" has " +
				to_string(currentStatement.args.size()) +
				" argument" + (currentStatement.args.size()==1?"":"s") + ", expected " +
				to_string(instr_arity_map[currentStatement.instr])
			).c_str());
		}

		switch (currentStatement.instr) {
		case Parser::INSTR_MOVE: {
			const Parser::Argument argument = currentStatement.args[0];
			const bool forwards = (bool) Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels).getInt();

			workTimeArg = (int) !forwards;

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
			dir += mod(Parser::evaluateExpression(argument, lineNumber, memoryMap, program->labels).getInt(), 4);
			break;
		}

		case Parser::INSTR_GOTO: {
			const Parser::Argument target = currentStatement.args[0];

			Parser::Position pos = Parser::itop(Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels).getInt());
			workTimeArg = (int) pos.page != curPage; // if page is same: 0, if different: 1

			memoryMap["_prevloc"] = Parser::ptoi({ curPage, curInstr + 1 });
			jumpTo(pos.page, pos.line);
			didJump = true;

			break;
		}

		case Parser::INSTR_IFGOTO: {
			const Parser::Argument condition = currentStatement.args[0];
			const Parser::Argument target = currentStatement.args[1];

			Parser::Position pos = Parser::itop(Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels).getInt());
			workTimeArg = (int) pos.page != curPage; // if page is same: 0, if different: 1

			// If the condition evaluates to 0, break.
			if (!Parser::evaluateExpression(condition, lineNumber, memoryMap, program->labels).getInt()) break;

			memoryMap["_prevloc"] = Parser::ptoi({ curPage, curInstr + 1 });
			jumpTo(pos.page, pos.line); // that's the `goto`
			didJump = true;

			break;
		}

		case Parser::INSTR_STO: {
			Parser::Argument varNameArgument = currentStatement.args[0];
			Parser::Argument valueArgument = currentStatement.args[1];

			if (varNameArgument.type != Parser::EN_VARIABLE) {
				// Wrong argument type(s).
				break;
			}

			Parser::Variable var = Parser::evaluateExpression(valueArgument, lineNumber, memoryMap, program->labels).toVar();
			storeVariable(varNameArgument.strval, var, lineNumber);
			break;
		}

		case Parser::INSTR_TRANS: {
			Parser::Argument pageIdArgument = currentStatement.args[0];
			Parser::Argument targetIdArgument = currentStatement.args[1];

			int fromId = Parser::evaluateExpression(pageIdArgument, lineNumber, memoryMap, program->labels).getInt();
			int toId = Parser::evaluateExpression(targetIdArgument, lineNumber, memoryMap, program->labels).getInt();
			const vector<Parser::Statement> &page = pages[fromId];

			const pair<int, int> targetLocation = calculateNextLocation(true);
			Bot *targetBot = board->at(targetLocation.first, targetLocation.second);

			if (targetBot != NULL) {
				//cerr << "copying to bot with id " << targetBot->index << endl;

				targetBot->copyPage(toId, page, targetBot->program->id == this->program->id);

				//cerr << "page copied" << endl;
			}

			workTimeArg = page.size();

			break;
		}

		case Parser::INSTR_PAGE: {
			Parser::Argument target = currentStatement.args[0];

			jumpTo(Parser::evaluateExpression(target, lineNumber, memoryMap, program->labels).getInt(), 0);
			didJump = true;
			break;
		}

		case Parser::INSTR_LOC: {
			Parser::Argument xTarget = currentStatement.args[0];
			Parser::Argument yTarget = currentStatement.args[1];

			if (xTarget.type == Parser::EN_VARIABLE && yTarget.type == Parser::EN_VARIABLE) {
				storeVariable(xTarget.strval, this->x, lineNumber);
				storeVariable(yTarget.strval, this->y, lineNumber);
			} else {
				// Wrong argument type.
			}

			break;
		}

		case Parser::INSTR_DIR: {
			Parser::Argument dTarget = currentStatement.args[0];

			int direction = getDir();

			if (dTarget.type == Parser::EN_VARIABLE) {
				storeVariable(dTarget.strval, direction, lineNumber);
			} else {
				// Wrong argument type.
			}

			break;
		}

		case Parser::INSTR_SUICIDE: {
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
			if (!this->board->insideBounds(targetLocation.first, targetLocation.second)) {
				response |= 1;
			} else {
				if (targetBot) {
					response |= 2;
					if (targetBot->program->id == this->program->id) response |= 4;
					if (targetBot->isAsleep) response |= 8;
				}
			}
			storeVariable(varNameArgument.strval, response, lineNumber);
			//cerr<<"Put "<<response<<" in "<<varNameArgument.strval<<" for bot "<<this->index<<endl;

			break;
		}

		case Parser::INSTR_TRANSLOCAL: {
			Parser::Argument pageIdArgument = currentStatement.args[0];
			Parser::Argument targetIdArgument = currentStatement.args[1];

			vector<Parser::Statement> page = pages[Parser::evaluateExpression(pageIdArgument, lineNumber, memoryMap, program->labels).getInt()];
			copyPage(Parser::evaluateExpression(targetIdArgument, lineNumber, memoryMap, program->labels).getInt(), page, true);
			workTimeArg = page.size();

			break;
		}

		case Parser::INSTR_BUILD: {
			const Parser::Argument tierArgument = currentStatement.args[0];
			int tier = Parser::evaluateExpression(tierArgument, lineNumber, memoryMap, program->labels).getInt();

			const pair<int, int> targetLocation = calculateNextLocation(true);

			if (board->canMoveTo(targetLocation.first, targetLocation.second)) {
				Bot bot(this, tier, targetLocation, board->nextIndex());
				board->addBot(bot);
			}

			workTimeArg = tier;

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

		case Parser::INSTR_PRINT: {
			Parser::Argument arg = currentStatement.args[0];
			const Parser::EvaluationResult res = Parser::evaluateExpression(arg, lineNumber, memoryMap, program->labels);
			cerr << index << '[' << curPage << '.' << curInstr << "]: " << res.toString() << endl;
			break;
		}

		case Parser::INSTR_PRINT_VARS: {
			for (const pair<string, Parser::Variable> &x : memoryMap) {
				cerr << "- " << x.first << ": ";
				cerr << x.second.toString();
				cerr << endl;
			}
			break;
		}

		case Parser::INSTR_STOP_MATCH: {
			cerr << "Bot with index " << index << " is stopping the match (INSTR_STOP_MATCH)" << endl;
			exit(0);
			break;
		}

		case Parser::INSTR_MAKEARR: {
			Parser::Argument arrayNameArgument = currentStatement.args[0];

			if (arrayNameArgument.type != Parser::EN_VARIABLE) {
				// Wrong argument type.
				break;
			}

			storeVariable(arrayNameArgument.strval, Parser::Variable::VAR_ARR, lineNumber);
			break;
		}

		case Parser::INSTR_AT: {
			Parser::Argument arrayNameArgument = currentStatement.args[0];
			Parser::Argument indexArgument = currentStatement.args[1];
			Parser::Argument varNameArgument = currentStatement.args[2];

			if (
				arrayNameArgument.type != Parser::EN_VARIABLE ||
				varNameArgument.type != Parser::EN_VARIABLE
			) {
				// Wrong argument type(s).
				break;
			}

			Parser::Variable *arrVar = getVariable(arrayNameArgument.strval);
			if (!arrVar || arrVar->type != Parser::Variable::VAR_ARR) {
				throw_error(lineNumber, "Given variable doesn't exist or isn't an array");
				break;
			}

			const int arrIndex = Parser::evaluateExpression(indexArgument, lineNumber, memoryMap, program->labels).getInt();

			if (arrIndex < 0 || arrIndex >= (int)arrVar->arrVal.size()) {
				storeVariable(varNameArgument.strval, Parser::Variable::VAR_NIL, lineNumber);
			} else {
				const Parser::Variable var = arrVar->arrVal.at(arrIndex);
				storeVariable(varNameArgument.strval, var, lineNumber);
			}

			break;
		}

		case Parser::INSTR_PUSH: {
			Parser::Argument arrayNameArgument = currentStatement.args[0];
			Parser::Argument valueArgument = currentStatement.args[1];

			if (arrayNameArgument.type != Parser::EN_VARIABLE) {
				// Wrong argument type.
				break;
			}

			Parser::Variable *var = getVariable(arrayNameArgument.strval);
			if (!var || var->type != Parser::Variable::VAR_ARR) {
				throw_error(lineNumber, "Given variable doesn't exist or isn't an array");
				break;
			}

			auto value = Parser::evaluateExpression(valueArgument, lineNumber, memoryMap, program->labels);
			var->arrVal.push_back(value.toVar());
			break;
		}

		case Parser::INSTR_DEL: {
			Parser::Argument arrayNameArgument = currentStatement.args[0];
			Parser::Argument indexArgument = currentStatement.args[1];

			if (
				arrayNameArgument.type != Parser::EN_VARIABLE ||
				indexArgument.type != Parser::EN_NUMBER
			) {
				// Wrong argument type.
				break;
			}

			Parser::Variable *var = getVariable(arrayNameArgument.strval);
			if (!var || var->type != Parser::Variable::VAR_ARR) {
				throw_error(lineNumber, "Given variable doesn't exist or isn't an array");
				break;
			}

			var->arrVal.erase(var->arrVal.begin() + indexArgument.intval);

			break;
		}

		case Parser::INSTR_LENGTH: {
			Parser::Argument arrayNameArgument = currentStatement.args[0];
			Parser::Argument varNameArgument = currentStatement.args[1];

			if (arrayNameArgument.type != Parser::EN_VARIABLE) {
				// Wrong argument type.
				break;
			}

			Parser::Variable *arrVar = getVariable(arrayNameArgument.strval);
			if (!arrVar || arrVar->type != Parser::Variable::VAR_ARR) {
				throw_error(lineNumber, "Given variable doesn't exist or isn't an array");
				break;
			}

			storeVariable(varNameArgument.strval, (int)arrVar->arrVal.size(), lineNumber);

			break;
		}

		case Parser::INSTR_BREAK: {
			cerr << "Breaking on breakpoint " << index << ':' << lineNumber << ", press enter to continue.";
			cin.get();
			break;
		}

		case Parser::INSTR_NOP:
		case Parser::INSTR_INVALID:
			break;
		}

		_workingFor = instructionWorkTime(currentStatement.instr, workTimeArg);
	} else {
		cerr << "Skipping instruction " << curInstr << " (instr_type " << currentStatement.instr << ") on bot with index " << index << " since it doesn't have an high enough tier (required: " << instr_tier_map.at(currentStatement.instr) << ", has: " << tier << ')' << endl;
	}

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

		if (_workingFor > 0) _workingFor--;

		curPage = pair.first;
		curInstr = pair.second;
		if (curPage >= (int)pages.size()) {
			assert(false);
		}
		if (curInstr >= (int)pages[curPage].size()) {
			isAsleep = true;
			curInstr = 0;
		}
	}

	/*cout << "nextTicked on bot with index " << index << " at " << curPage << "." << curInstr << ", dead: " << (int) isDead << "; asleep: " << (int) isAsleep << "; next instr to exec: ";
	if(curInstr >= (int)pages[curPage].size())cout << "--end of page--";
	else cout << pages[curPage][curInstr].instr;
	cout << endl;*/
	return isDead;
}
