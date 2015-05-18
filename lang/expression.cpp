#include "expression.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace Parser {

	vector<ExprToken> tokeniseExpression(const string &s){
		const char *const whitespace=
			"\x01\x02\x03\x04\x05\x06\a\b\t\x0a\v\f\x0d\x0e\x0f\n\x11\x12\r\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f ";
		const char *const wordchars=
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
		const char *const numberchars="0123456789";
		int start,end,cursor=0,i;
		vector<ExprToken> tkns;
		while(true){
			start=s.find_first_not_of(whitespace,cursor);
			if(start==string::npos)return tkns;
			ExprToken token;
			if(strchr(wordchars,s[start])!=NULL){
				token.type=ETT_WORD;
				end=s.find_first_not_of(wordchars,start+1);
			} else if(strchr(numberchars,s[start])!=NULL){
				token.type=ETT_NUMBER;
				end=s.find_first_not_of(numberchars,start+1);
			} else if(strchr(whitespace,s[start])!=NULL){
				cursor=s.find_first_not_of(whitespace,start+1);
				continue;
			} else {
				token.type=ETT_SYMBOL;
				for(i=start;i<(int)s.size();i++){
					if(strchr(wordchars,s[i])!=NULL||
					   strchr(numberchars,s[i])!=NULL||
					   strchr(whitespace,s[i])!=NULL)break;
				}
				end=i;
			}
			token.val=s.substr(start,end-start);
			tkns.push_back(token);
			if(end==s.size())break;
			cursor=end;
		}
		return tkns;
	}

	ExprNode parseExpression(const vector<ExprToken> &tkns){
		;
	}

};
