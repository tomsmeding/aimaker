namespace Parser{

enum Instruction{
	INSTR_MOVE,
	INSTR_STO
}

Instruction instr_string(string word);

class Statement{
public:
	Instruction instr;
	vector<Argument> args;
};

vector<string> lex(string);
vector<Statement> parser(vector<string>);

}
