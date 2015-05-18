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

		EN_NOT, // ! (right-unary)

		EN_NUMBER,
		EN_VARIABLE
	};

	struct ExprNode {
		ExprNodeType type;
		ExprNode *left,*right;
	};

	vector<ExprToken> tokeniseExpression(const string &s);
	ExprNode parseExpression(const string &s);

};
