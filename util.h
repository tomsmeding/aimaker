#pragma once

#include <string>
#include <vector>

using namespace std;

int genid(void) {
	static id = 0;
	return id++;
}

vector<string> split(const string &s,const char delim){
	string::size_type cursor=0;
	string::size_type idx=s.find(delim);
	vector<string> vec;
	while(idx!=string::npos){
		vec.push_back(s.substr(cursor,idx-cursor));
		cursor=idx+1;
		idx=s.find(delim,cursor);
	}
	return vec;
}

string trim(const string &s){
	string::size_type begin=s.find_first_not_of(" \t"),
	                  end=s.find_last_not_of(" \t");
	return s.substr(begin,end-begin);
}
