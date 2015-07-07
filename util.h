#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

int genid(void);

vector<string> split(const string&, const char, int numsplits=-1);
vector<string> split(const string &s, const char* delims, int numsplits);

string trim(const string&);

bool is_numeric(const string&);

void throw_error(int, const char*);

void to_lower(string &original);

template <typename T>
void print_vector(const vector<T> &vec);
