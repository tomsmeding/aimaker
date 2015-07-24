#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "parser.h"

using namespace std;

namespace Parser {

	struct LabelInfo;
	struct EvaluationResult;
	typedef unordered_map<string, LabelInfo> LabelMap;

	struct Variable {
		enum VariableType {
			VAR_ARR,
			VAR_INT,
			VAR_NIL,
			VAR_STRING
		};

		VariableType type;
		vector<Variable> arrVal;
		string strVal;
		int32_t intVal;

		Variable(void);
		Variable(int32_t);
		Variable(string);
		Variable(VariableType);

		int getSize(void) const;
		string toString(void) const;

		EvaluationResult toER(int) const;
	};

	enum ExprTokenType {
		ETT_WORD, ETT_NUMBER, ETT_LABEL, ETT_SYMBOL, ETT_STRING
	};

	struct ExprToken {
		ExprTokenType type;
		string val;
	};

	enum ExprNodeType {
		EN_ADD=0, // +
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
		EN_SHIFTR, // >>
		EN_SHIFTL, // <<

		EN_NOT, // ! (right-unary)
		EN_NEGATE, // - (right-unary)

		EN_PAREN1, // (
		EN_PAREN2, // )

		EN_NUMBER, // [0-9]+
		EN_STRING, // "[^"]*"
		EN_VARIABLE, // [a-zA-Z_][a-zA-Z0-9_]*
		EN_LABEL, // @[a-zA-Z0-9_]+

		EN_NIL, // nil

		EN_SUBTRACT_OR_NEGATE_CONFLICT,
		EN_INVALID
	};

	struct ExprNode {
		ExprNodeType type;
		ExprNode *left,*right;
		string strval;
		int intval;
		int hasval; //0=none, 1=string, 2=int
		ExprNode(void);
		ExprNode(const ExprNode&);
		ExprNode& operator=(const ExprNode&);
		ExprNode(ExprNode&&);
		ExprNode& operator=(ExprNode&&);
		ExprNode(ExprNodeType);
		ExprNode(ExprNodeType,const string&);
		ExprNode(ExprNodeType,const int);
		ExprNode(ExprNodeType,ExprNode*,ExprNode*,const string&);
		ExprNode(ExprNodeType,ExprNode*,ExprNode*,const int);
		~ExprNode(void);
		void setNullChildren(void);
	};

	bool leftAssoc(ExprNodeType);
	int precedence(ExprNodeType);
	const char* operatorToString(ExprNodeType);

	ExprNodeType interpretOperator(const string&);

	// These two throw char* for invalid operator.
	vector<ExprToken> tokeniseExpression(const string&, const int);
	void parseExpression(/*out*/ExprNode*, const vector<ExprToken>&, const int);

	struct EvaluationResult {
	private:
		int lineNumber;
	public:
		enum ResultType {
			RES_NIL,
			RES_NUMBER,
			RES_STRING,
			// no RES_ARR, since we don't have expressions to create an array,
			// arrays can currently only be created using `makearr` or `arr`.
			RES_VAR
		};

		EvaluationResult();
		EvaluationResult(int);

		ResultType type;
		int intVal;
		string strVal;
		const Variable *varVal;

		int getInt() const;
		string getString() const;
		Variable toVar(void) const;
		string toString(void) const;
	};

	int runExprNodeFunction(const ExprNodeType, const ExprNode*, const ExprNode*, const int, const unordered_map<string, Variable>&, const LabelMap&);

	// Throws if a variable in the expression is not found in the map.
	EvaluationResult evaluateExpression(const ExprNode&, const int lineNumber, const unordered_map<string, Variable>&, const LabelMap&);
};
