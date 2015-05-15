#include "util.h"

using namespace std;

int genid(void) {
	static int id = 0;
	return id++;
}

vector<string> split(const string &s,const char delim,int numsplits){
	string::size_type cursor=0;
	string::size_type idx=s.find(delim);
	vector<string> vec;
	while(true){
		vec.push_back(s.substr(cursor,idx-cursor));
		if(idx==string::npos||numsplits==0)break;
		numsplits--;
		cursor=idx+1;
		idx=s.find(delim,cursor);
	}
	vec.push_back(s.substr(cursor,string::npos));
	return vec;
}

string trim(const string &s){
	string::size_type begin=s.find_first_not_of(" \t"),
	                  end=s.find_last_not_of(" \t");
	return s.substr(begin,end-begin);
}

bool char_is_digit(const char c){
	return isdigit(c);
}

bool is_numberic(const string &s) {
	return all_of(s.begin(), s.end(), char_is_digit);
}

void throw_error(int lineNumber, const char *message) {
	char *error;
	asprintf(&error, "Error at line %d: %s", lineNumber, message);
	throw error;
}

void to_lower(string &original) {
	std::transform(original.begin(), original.end(), original.begin(), ::tolower);
}
