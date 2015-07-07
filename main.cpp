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
	cerr << "\t--maxbotmemory=<int> | Sets the max memory a bot can store." << endl;
	cerr << "\t--parseonly | Quits after parsing the program(s)." << endl;
}

bool parseFlagOption(const string &s) { // True if flag, otherwise false.
	if (s.find("--") == 0) {
		string trimmed = trim(s.substr(2, s.size() - 2));
		vector<string> splitted = split(trimmed, "= ", 0);

		if (splitted[0] == "maxbotmemory") {
			params.maxBotMemory = stoi(splitted[1]);
		} else if (splitted[0] == "parseonly") {
			params.parseonly = true;
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
		int i;
		Board board(5);
		vector<Parser::Program> programs;
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

			cerr << "Reading in program " << i - 1 << ": '" << argv[i] << "'... ";

			try {
				vector<string> contents;
				try {
					contents = readFile(argv[i]);
				} catch (const char *str) {
					cerr << str << endl << endl;

					printusage(argc, argv);
					return 1;
				}

				cerr << "Read contents... ";
				Parser::Program program = Parser::parse(argv[1], contents);
				cerr << "Parsed... ";
				programs.push_back(program);
			} catch (const char *str) {
				cerr << "ERROR: " << str << endl;
				return 1;
			}
			board.bots.emplace_back(&programs[programs.size() - 1], &board, make_pair(0, 0));
			cerr << "Done." << endl;
		}
		const int numprogs = programs.size();

		cerr << "Done reading in programs" << endl;

		if (params.parseonly) {
			return 0;
		}

		int tick = 0, progid;
		bool stillthere[numprogs];
		memset(stillthere, 0, numprogs * sizeof(bool));
		bool endgame = false;

		clearScreen();
		cout << board.render() << endl;
		tick++;
		usleep(1000 * 1000);

		while (true) {
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

			int botindex=0;
			for (Bot &b : board.bots) {
				b.nextTick();
				cerr<<"nextticking bot "<<botindex<<" to ("<<b.getPos().first<<','<<b.getPos().second<<") direction "<<b.getDir()<<endl;
				botindex++;
			}

			clearScreen();
			cout << board.render() << endl;
			cout << "tick " << tick << endl;
			tick++;
			usleep(300000);
		}
	} catch (char *msg) {
		cerr << "ERROR CAUGHT: " << msg << endl;
	}

	return 0;
}
