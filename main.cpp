#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>
#include "lang/parser.h"
#include "board.h"
#include "bot.h"

using namespace std;

int MaxBotMemory = 50;

void clearScreen(void) {
	cout << "\x1B[2J" << flush;
}

vector<string> readFile(const char *const fname) {
	ifstream f(fname);
	assert(!!f);
	/*f.seekg(0,ios_base::end);
	int filelen=f.tellg();
	f.seekg(0,ios_base::beg);
	string buf;
	buf.resize(filelen);
	f.read(buf.data(),filelen);
	f.close();*/
	vector<string> lines;
	string line;
	while (f) {
		getline(f, line);
		lines.push_back(move(line));
	}
	return lines;
}

void printusage(int argc, char **argv) {
	cerr << "Usage: " << argv[0] << " <options> <botprograms...>" << endl;
	cerr << "Options:" << endl;
	cerr << "\t--maxbotmemory=<int>" << endl;
}

bool parseFlagOption(const string &s) { // True if flag, otherwise false.
	if (s.find("--") != string::npos) {
		string trimmed = trim(s.substr(2, s.size() - 2));
		vector<string> splitted = split(s, (char *)"= ", 0);

		if (splitted[0] == "maxbotmemory") {
			MaxBotMemory = stoi(splitted[1]);
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

	int i;
	Board board(20);
	vector<Parser::Program> programs;
	vector<Bot> bots;
	for (i = 1; i < argc; i++) {
		cerr << "Reading in program " << i << ": '" << argv[i] << "'... ";

		try {
			if (parseFlagOption(argv[i])) {
				// Correct flag given. Continue since this isn't a bot.
				continue;
			}
		} catch (const char *str) { // Unknown flag given.
			printusage(argc, argv);
			return 1;
		}

		try {
			vector<string> contents = readFile(argv[i]);
			cerr << "Read contents... ";
			Parser::Program program = Parser::parse(argv[1], contents);
			cerr << "Parsed... ";
			programs.push_back(program);
		} catch (const char *str) {
			cerr << "ERROR: " << str << endl;
			return 1;
		}
		bots.emplace_back(&programs[programs.size() - 1], &board, make_pair(0, 0));
		cerr << "Done." << endl;
	}
	const int numprogs = programs.size();

	cerr << "Done reading in programs" << endl;

	int tick = 0;
	bool stillthere[numprogs];
	int progid;
	memset(stillthere, 0, numprogs * sizeof(bool));
	bool endgame = false;
	while (true) {
		for (Bot &b : bots) {
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
		if (endgame)break;
		for (Bot &b : bots) {
			b.nextTick();
		}
		clearScreen();
		cout << board.render() << endl;
		cout << "tick " << tick << endl;
	}
	return 0;
}
