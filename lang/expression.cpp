#include "expression.h"
#include <vector>
#include <iostream>

using namespace std;

namespace Parser {

	ExprNode::ExprNode(ExprNodeType _t)
		:type(_t),left(NULL),right(NULL),strval(NULL),intval(0),hasval(false){}
	ExprNode::ExprNode(ExprNodeType _t,const string &_s)
		:type(_t),left(NULL),right(NULL),intval(0),hasval(true){
			const int len=_s.size();
			strval=new char[len];
			memcpy(strval,_s.data(),len);
		}
	ExprNode::ExprNode(ExprNodeType _t,const int _i)
		:type(_t),left(NULL),right(NULL),strval(NULL),intval(_i),hasval(true){}
	ExprNode::ExprNode(ExprNodeType _t,ExprNode *_l,ExprNode *_r,const string &_s)
		:type(_t),left(_l),right(_r),intval(0),hasval(true){
			const int len=_s.size();
			strval=new char[len];
			memcpy(strval,_s.data(),len);
		}
	ExprNode::ExprNode(ExprNodeType _t,ExprNode *_l,ExprNode *_r,const int _i)
		:type(_t),left(_l),right(_r),strval(NULL),intval(_i),hasval(true){}
	ExprNode::~ExprNode(void){
		if(strval)delete[] strval;
	}


	bool leftAssoc(ExprNodeType type){
		switch(type){
			case EN_NOT:
			case EN_NEGATE:
				return false;
			default:
				return true;
		}
	}
	int precedence(ExprNodeType type){ //the tens are c++ precedence
		cerr<<"precedence("<<operatorToString(type)<<')'<<endl;
		switch(type){
			case EN_NOT:
			case EN_NEGATE:
				return 30;
			case EN_MULTIPLY:
			case EN_DIVIDE:
				return 50;
			case EN_ADD:
			case EN_SUBTRACT:
				return 60;
			case EN_SHIFTR:
			case EN_SHIFTL:
				return 70;
			case EN_GREATER:
			case EN_GREATEREQUAL:
			case EN_LESS:
			case EN_LESSEQUAL:
				return 80;
			case EN_EQUALS:
			case EN_NOTEQUAL:
				return 90;
			case EN_BITAND:
				return 100;
			case EN_BITXOR:
				return 110;
			case EN_BITOR:
				return 120;
			case EN_AND:
				return 130;
			case EN_XOR:
				return 135;
			case EN_OR:
				return 140;

			case EN_PAREN1:
			case EN_PAREN2:
				return 999999;

			case EN_NUMBER:
			case EN_VARIABLE:
			case EN_SUBTRACT_OR_NEGATE_CONFLICT:
			case EN_INVALID:
				return -1; //eh?
		}
	}

	ExprNodeType interpretOperator(const string &s){
		if(s=="+")return EN_ADD;
		//if(s=="-")return EN_SUBTRACT;
		if(s=="*")return EN_MULTIPLY;
		if(s=="/")return EN_DIVIDE;
		if(s=="&")return EN_BITAND;
		if(s=="|")return EN_BITOR;
		if(s=="^")return EN_BITXOR;
		if(s=="==")return EN_EQUALS;
		if(s=="!=")return EN_NOTEQUAL;
		if(s==">")return EN_GREATER;
		if(s==">=")return EN_GREATEREQUAL;
		if(s=="<")return EN_LESS;
		if(s=="<=")return EN_LESSEQUAL;
		if(s=="&&")return EN_AND;
		if(s=="||")return EN_OR;
		if(s=="^^")return EN_XOR;
		if(s==">>")return EN_SHIFTR;
		if(s=="<<")return EN_SHIFTL;
		if(s=="!")return EN_NOT;
		//if(s=="-")return EN_NEGATE;
		if(s=="(")return EN_PAREN1;
		if(s==")")return EN_PAREN2;
		if(s=="-")return EN_SUBTRACT_OR_NEGATE_CONFLICT;
		return EN_INVALID;
	}
	const char* operatorToString(ExprNodeType type){
		switch(type){
		case EN_ADD: return "EN_ADD";
		case EN_SUBTRACT: return "EN_SUBTRACT";
		case EN_MULTIPLY: return "EN_MULTIPLY";
		case EN_DIVIDE: return "EN_DIVIDE";
		case EN_BITAND: return "EN_BITAND";
		case EN_BITOR: return "EN_BITOR";
		case EN_BITXOR: return "EN_BITXOR";
		case EN_EQUALS: return "EN_EQUALS";
		case EN_NOTEQUAL: return "EN_NOTEQUAL";
		case EN_GREATER: return "EN_GREATER";
		case EN_GREATEREQUAL: return "EN_GREATEREQUAL";
		case EN_LESS: return "EN_LESS";
		case EN_LESSEQUAL: return "EN_LESSEQUAL";
		case EN_AND: return "EN_AND";
		case EN_OR: return "EN_OR";
		case EN_XOR: return "EN_XOR";
		case EN_SHIFTR: return "EN_SHIFTR";
		case EN_SHIFTL: return "EN_SHIFTL";
		case EN_NOT: return "EN_NOT";
		case EN_NEGATE: return "EN_NEGATE";
		case EN_PAREN1: return "EN_PAREN1";
		case EN_PAREN2: return "EN_PAREN2";
		case EN_NUMBER: return "EN_NUMBER";
		case EN_VARIABLE: return "EN_VARIABLE";
		case EN_SUBTRACT_OR_NEGATE_CONFLICT: return "EN_SUBTRACT_OR_NEGATE_CONFLICT";
		case EN_INVALID: return "EN_INVALID";
		}
	}


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
			if(strchr(whitespace,s[start])!=NULL){
				cursor=s.find_first_not_of(whitespace,start+1);
				continue;
			} else if(strchr(wordchars,s[start])!=NULL){
				token.type=ETT_WORD;
				end=s.find_first_not_of(wordchars,start+1);
			} else if(strchr(numberchars,s[start])!=NULL){
				token.type=ETT_NUMBER;
				end=s.find_first_not_of(numberchars,start+1);
			} else {
				token.type=ETT_SYMBOL;
				for(i=start+1;i<(int)s.size()&&i-start<=2;i++){
					if(strchr(wordchars,s[i])!=NULL||
					   strchr(numberchars,s[i])!=NULL||
					   strchr(whitespace,s[i])!=NULL)break;
				}
				for(;i>start;i--)
					if(interpretOperator(s.substr(start,i-start))!=EN_INVALID)break;
				end=i;
			}
			token.val=s.substr(start,end-start);
			if(token.type==ETT_SYMBOL&&interpretOperator(token.val)==EN_INVALID){
				char *buf;
				asprintf(&buf,"Invalid operator '%s'",token.val.c_str());
				throw buf;
			}
			tkns.push_back(token);
			if(end==s.size())break;
			cursor=end;
		}
		return tkns;
	}

	void printstack(const vector<ExprNode> &stack){
		const ExprNode *node;
		for(int i=0;i<(int)stack.size();i++){
			node=&stack[i];
			cerr<<i<<": "<<operatorToString(node->type)<<' ';
			if(node->hasval){
				if(node->strval)cerr<<node->strval;
				else cerr<<node->intval;
			}
			cerr<<endl;
		}
	}
	ExprNode parseExpression(const vector<ExprToken> &tkns){
		//this implements Dijkstra's Shunting Yard algorithm, skipping functions.
		vector<ExprNode> nodestack,opstack;
		ExprNodeType type;
		int i;
		bool lastWasOperator=true,foundLeftParen,isleftassoc;
		int prec;
		for(i=0;i<(int)tkns.size();i++){
			const ExprToken token=tkns[i];
			switch(token.type){
			case ETT_NUMBER:
				nodestack.emplace_back(EN_NUMBER,stoi(token.val));
				lastWasOperator=false;
				break;
			case ETT_WORD:
				nodestack.emplace_back(EN_VARIABLE,token.val);
				lastWasOperator=false;
				break;
			case ETT_SYMBOL:
				type=interpretOperator(token.val);
				if(type==EN_INVALID){
					char *buf;
					asprintf(&buf,"Invalid operator '%s'",token.val.c_str());
					throw buf;
				}
				if(type==EN_SUBTRACT_OR_NEGATE_CONFLICT){
					if(lastWasOperator)type=EN_NEGATE;
					else type=EN_SUBTRACT;
				}
				if(type==EN_PAREN1){
					opstack.emplace_back(type);
				} else if(type==EN_PAREN2){
					foundLeftParen=false;
					while(opstack.size()){
						const ExprNode opnode=opstack[opstack.size()-1];
						opstack.pop_back();
						if(opnode.type==EN_PAREN1){
							foundLeftParen=true;
							break;
						}
						nodestack.push_back(opnode);
					}
					if(!foundLeftParen){
						char *buf;
						asprintf(&buf,"Closing parenthesis without matching open '('");
						throw buf;
					}
				} else {
					isleftassoc=leftAssoc(type);
					prec=precedence(type);
					cerr<<"prec="<<prec<<" type="<<operatorToString(type)<<endl;
					while(opstack.size()){
						const ExprNode opnode=opstack[opstack.size()-1];
						//below condition inequality operators are reversed because it BREAKS if true, AND because precedence numbers are reversed from the source wikipedia article. Watch your steps.
						if(isleftassoc?
								prec<precedence(opnode.type):
								prec<=precedence(opnode.type))
							break;
						nodestack.push_back(opnode);
						opstack.pop_back();
					}
					opstack.emplace_back(interpretOperator(token.val));
				}
				lastWasOperator=true;
				break;
			}
			/*cerr<<"Opstack:"<<endl;
			printstack(opstack);
			cerr<<"Nodestack:"<<endl;
			printstack(nodestack);
			cerr<<"------------"<<endl;*/
		}
		while(opstack.size()){
			nodestack.push_back(opstack[opstack.size()-1]);
			opstack.pop_back();
		}
		//cerr<<"---\nFINAL:"<<endl;
		cerr<<"Opstack:"<<endl;
		printstack(opstack);
		cerr<<"Nodestack:"<<endl;
		printstack(nodestack);
		cerr<<"------------"<<endl;
	}

};

#ifdef EXPRESSION_DEBUG_MAIN
int main(void){
	try {
		Parser::parseExpression(Parser::tokeniseExpression("(1+2)*3&&1==2"));
	} catch(char *exc){
		cerr<<"EXCEPTION: "<<exc<<endl;
	}
}
#endif
