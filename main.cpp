#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include "lang/parser.h"
#include "board.h"
#include "bot.h"
#include "io_util.h"

using namespace std;

vector<string> readFile(const char *const fname){
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
	while(f){
		getline(cin,line);
		lines.push_back(move(line));
	}
	return lines;
}

void printusage(int argc,char **argv){
	cerr<<"Usage: "<<argv[0]<<" <botprograms...>"<<endl;
}

int main(int argc,char **argv){
	if(argc==1){
		printusage(argc,argv);
		return 1;
	}
	int i;
	Board board(20);
	vector<Parser::Program> programs;
	vector<Bot> bots;
	for(i=1;i<argc;i++){
		try {
			programs.push_back(Parser::parser(readFile(argv[i]));
		} catch(const char *str){
			cerr<<"ERROR: "<<str<<endl;
			return 1;
		}
		bots.emplace_back(&programs[programs.size()-1],&board);
	}
	const int numprogs=programs.size();

	int tick=0;
	bool stillthere[numprogs];
	int progid;
	memset(stillthere,0,numprogs*sizeof(bool));
	bool endgame=false;
	while(true){
		for(Bot &b : bots){
			progid=b.program->id;
			for(i=0;i<numprogs;i++){
				if(programs[i].id==progid){
					stillthere[i]=true;
					break;
				}
			}
		}
		for(i=0;i<numprogs;i++){
			if(stillthere[i])continue;
			cout<<"Program "<<i<<'('<<programs[i].name<<") has no bots left!"<<endl;
			endgame=true;
		}
		if(endgame)break;
		for(Bot &b : bots){
			b.nextTick();
		}
		clearScreen();
		cout<<board.render()<<endl;
	}
	return 0;
}
