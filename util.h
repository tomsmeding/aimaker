#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

int genid(void);

vector<string> split(const string&,const char,int numsplits=-1);

string trim(const string&);

bool is_numberic(const string&);
