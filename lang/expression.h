#include <string>
#include <vector>

using namespace std;

namespace Parser {

	enum ExprTokenType {
		ETT_WORD, ETT_NUMBER, ETT_SYMBOL
	};

	struct ExprToken {
		ExprTokenType type;
		string val;
	};

	enum ExprNodeType {
		EN_ADD, // +
		EN_SUBTRACT, // -
		EN_MULTIPLY, // *
		EN_DIVIDE, // /

		EN_BITAND, // &
		EN_BITOR, // |
		EN_BITXOR, // ^

		EN_EQUALS, // ==
		EN_NOTEQUAL, // !=
		EN_GREATER, // >
		EN_GREATEREQUAL, // >=
		EN_LESS, // <
		EN_LESSEQUAL, // <=

		EN_AND, // &&
		EN_OR, // ||
		EN_XOR, // ^^
		EN_SHIFTR, // >>
		EN_SHIFTL, // <<

		EN_NOT, // ! (right-unary)
		EN_NEGATE, // - (right-unary)

		EN_PAREN1, // (
		EN_PAREN2, // )

		EN_NUMBER, // [0-9]+
		EN_VARIABLE, // [a-zA-Z_][a-zA-Z0-9_]*

		EN_SUBTRACT_OR_NEGATE_CONFLICT,
		EN_INVALID
	};

	struct ExprNode {
		ExprNodeType type;
		ExprNode *left,*right;
		string strval;
		int intval;
		int hasval; //0=none, 1=string, 2=int
		ExprNode(ExprNodeType);
		ExprNode(ExprNodeType,const string&);
		ExprNode(ExprNodeType,const int);
		ExprNode(ExprNodeType,ExprNode*,ExprNode*,const string&);
		ExprNode(ExprNodeType,ExprNode*,ExprNode*,const int);
	};

	bool leftAssoc(ExprNodeType);
	int precedence(ExprNodeType);
	const char* operatorToString(ExprNodeType);

	ExprNodeType interpretOperator(const string&);

	//these two throw char* for invalid operator
	vector<ExprToken> tokeniseExpression(const string&);
	ExprNode parseExpression(const vector<ExprToken>&);

};
