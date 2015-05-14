namespace Parser {

class Statement {
public:
	Instruction instr;

}

vector<string> lex(string);
Vector<Statement> parser(vector<string>);

}
