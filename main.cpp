#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "unistd.h"
#include "lang/parameters.h"
#include "lang/parser.h"
#include "board.h"
#include "bot.h"
#include "botdist.h"

using namespace std;

extern Params params;

void clearScreen(void) {
	cout << "\x1B[2J\x1B[H" << flush;
}

vector<string> readFile(const char *const fname) {
	ifstream f(fname);
	vector<string> lines;
	if (!!f) {
		/*f.seekg(0,ios_base::end);
		int filelen=f.tellg();
		f.seekg(0,ios_base::beg);
		string buf;
		buf.resize(filelen);
		f.read(buf.data(),filelen);
		f.close();*/
		string line;
		while (f) {
			getline(f, line);
			lines.push_back(move(line));
		}
		return lines;
	} else {
		char *message;
		asprintf(&message, "Can't read program '%s', are you sure it exists?", fname);
		throw message;
	}
}

void printusage(int argc, char **argv) {
	cerr << "Usage: " << argv[0] << " <options> <botprograms...>" << endl;
	cerr << "Options:" << endl;
	cerr << "\t--boardsize=<int> (50) | Sets the size of the game board." << endl;
	cerr << "\t--maxbotmemory=<int> (5) | Sets the max memory a bot can store." << endl;
	cerr << "\t--maxpages=<int> | Sets the max pages a program can have." << endl;
	cerr << "\t--parseonly (16) | Quits after parsing the program(s)." << endl;
}

bool parseFlagOption(const string &s) { // True if flag, otherwise false.
	if (s.find("--") == 0) {
		string trimmed = trim(s.substr(2, s.size() - 2));
		vector<string> splitted = split(trimmed, '=', 1);

		if (splitted[0] == "maxbotmemory") {
			params.maxBotMemory = stoi(splitted[1]);
		} else if (splitted[0] == "parseonly") {
			params.parseOnly = true;
		} else if (splitted[0] == "boardsize") {
			params.boardSize = stoi(splitted[1]);
		} else if (splitted[0] == "maxpages") {
			params.maxPages = stoi(splitted[1]);
		} else {
			char *message;
			asprintf(&message, "Unknown flag '%s'.", splitted[0].c_str());
			throw message;
		}

		return true;
	} else {
		return false;
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		printusage(argc, argv);
		return 1;
	}

	try {
		vector<string> programNames;
		int i;
		for (i = 1; i < argc; i++) {
			try {
				if (parseFlagOption(argv[i])) {
					// Correct flag given. Continue since this isn't a bot.
					continue;
				}
			} catch (const char *str) { // Unknown flag given.
				printusage(argc, argv);
				return 1;
			}

			programNames.push_back(argv[i]);
		}

		Board board(params.boardSize);
		vector<Parser::Program> programs;
		programs.reserve(programNames.size());

		for (i = 0; i < (int)programNames.size(); i++) {
			const string &programName = programNames[i];

			cerr << "Reading in program " << i << ": '" << programName << "'... ";

			try {
				vector<string> contents;
				try {
					contents = readFile(programName.c_str());
				} catch (const char *str) {
					cerr << str << endl << endl;

					printusage(argc, argv);
					return 1;
				}

				cerr << "Read contents... ";
				Parser::Program program = Parser::parse(programName.c_str(), contents);
				cerr << "Parsed... ";
				programs.push_back(program);
			} catch (const char *str) {
				cerr << "ERROR: " << str << endl;
				return 1;
			}
			board.bots.emplace_back(&programs[programs.size() - 1], &board, make_pair(0, 0), board.bots.size());
			cerr << "Done." << endl;
		}
		const int numprogs = programs.size();

		cerr << "Done reading in programs" << endl;


		vector<int> botDist = makeBotDistribution(params.boardSize, params.boardSize, board.bots.size());
		for (i = 0; i < (int)botDist.size(); i++) {
			int loc = botDist[i];
			Bot &bot = board.bots[i];

			bot.x = loc % params.boardSize;
			bot.y = loc / params.boardSize;
		}

		if (params.parseOnly) {
			return 0;
		}

		int progid;
		bool stillthere[numprogs];
		bool endgame = false;

		clearScreen();
		cout << board.render() << endl;
		usleep(1000 * 1000);


		while (true) {
			memset(stillthere, 0, numprogs * sizeof(bool));
			for (Bot &b : board.bots) {
				progid = b.program->id;

				for (i = 0; i < numprogs; i++) {
					if (programs[i].id == progid) {
						stillthere[i] = true;
						break;
					}
				}
			}

			for (i = 0; i < numprogs; i++) {
				if (stillthere[i])continue;
				cout << "Program " << i << '(' << programs[i].name << ") has no bots left!" << endl;
				endgame = true;
			}

			if (endgame) {
				break;
			}

			board.nextTick();

			clearScreen();
			cout << board.render() << endl;
			cout << "tick " << board.currentTick() << endl;
			usleep(200000);
		}
	} catch (char *msg) {
		cerr << "ERROR CAUGHT: " << msg << endl;
	}

	return 0;
}
