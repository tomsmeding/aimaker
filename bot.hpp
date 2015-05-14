#include <unordered_map>
#include <vector>
#include "lang/parser.h"

using namespace std;

class Bot {
private:
	int curPage;
	int curInstr;

public:
	int id;
	unordered_map<string, int> memoryMap;
	vector<Parser::Statement> pages[16];
};
