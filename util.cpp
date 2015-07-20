#include <climits>
#include "util.h"

using namespace std;

int genid(void) {
	static int id = 0;
	return id++;
}

vector<string> split(const string &s, const char delim, int numsplits) {
	string::size_type cursor = 0;
	string::size_type idx = s.find(delim);
	vector<string> vec;
	while (idx != string::npos && numsplits != 0) {
		vec.push_back(s.substr(cursor, idx - cursor));
		numsplits--;
		cursor = idx + 1;
		idx = s.find(delim, cursor);
	}
	vec.push_back(s.substr(cursor, string::npos));
	return vec;
}

vector<string> split(const string &s, const char *delims, int numsplits) {
	string::size_type cursor = 0;
	unsigned long long idx = UINT_MAX;
	for(const char *p = delims; *p; p++) idx = min(idx, (unsigned long long)s.find(*p));
	vector<string> vec;
	while (idx != string::npos && numsplits != 0) {
		vec.push_back(s.substr(cursor, idx - cursor));
		numsplits--;
		cursor = idx + 1;
		idx = UINT_MAX;
		for(const char *p = delims; *p; p++) idx = min(idx, (unsigned long long)s.find(*p));
	}
	vec.push_back(s.substr(cursor, string::npos));
	return vec;
}

string trim(const string &s) {
	string::size_type begin = s.find_first_not_of(" \t"),
					  end = s.find_last_not_of(" \t");
	if (begin == string::npos)return string();
	else return s.substr(begin, end - begin + 1);
}

bool char_is_digit(const char c) {
	return isdigit(c);
}

bool is_numeric(const string &s) {
	return all_of(s.begin(), s.end(), char_is_digit);
}

void throw_error(int lineNumber, const char *message) {
	char *error;

	if (lineNumber > -1) {
		asprintf(&error, "Error at line %d: %s", lineNumber, message);
	} else {
		asprintf(&error, "%s", message);
	}

	throw error;
}

void to_lower(string &original) {
	std::transform(original.begin(), original.end(), original.begin(), ::tolower);
}

int mod(int a, int b) {
	return (a % b + b) % b;
}
