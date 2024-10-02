// =======================================================
// Name        : lex.cpp
// Description : Takes in text to be converted to tokens
// =======================================================

#include "lex.h"

string enumToString[44] = {
	"IF",	 "ELSE",   "PRINT",	   "INT",	   "FLOAT",	   "CHAR",	   "STRING",   "BOOL",	 "PROGRAM", "TRUE", "FALSE",
	"IDENT", "ICONST", "RCONST",   "SCONST",   "BCONST",   "CCONST",   "PLUS",	   "MINUS",	 "MULT",	"DIV",	"ASSOP",
	"EQ",	 "NEQ",	   "ADDASSOP", "SUBASSOP", "MULASSOP", "DIVASSOP", "REMASSOP", "GTHAN",	 "LTHAN",	"AND",	"OR",
	"NOT",	 "REM",	   "COMMA",	   "SEMICOL",  "LPAREN",   "RPAREN",   "DOT",	   "LBRACE", "RBRACE",	"ERR",	"DONE"};

ostream &operator<<(ostream &out, const LexItem &tok) {
	Token token = tok.GetToken();

	// Output the elements as strings rather than ints
	out << enumToString[token];

	if (token == ICONST || token == RCONST || token == BCONST) {
		out << ": (" << tok.GetLexeme() << ")";
	} else if (token == IDENT) {
		out << ": <" << tok.GetLexeme() << ">";
	} else if (token == SCONST) {
		out << ": \"" << tok.GetLexeme() << "\"";
	} else if (token == CCONST) {
		out << ": \'" << tok.GetLexeme() << "\'";
	} else if (token == ERR) {
		out << "ERR: Unrecognized Lexeme {" << tok.GetLexeme() << "} in line " << tok.GetLinenum();
	}

	out << endl;
	return out;
}

/*LexItem id_or_kw(const string &lexeme, int linenum) {}*/

LexItem getNextToken(istream &in, int &linenum) {
	LexItem lex = LexItem();

	// States of the lexer, used to determine multi-character tokens
	enum STATES { START, IN_IDENT, IN_INT, IN_NUM, IN_STR, IN_COMMENT };

	// Continually consumes characters to find lexemes
	string lexeme;
	char c;
	STATES curState = START;

	// TODO: everytime a \n is read, increment linenum

	while ((c = in.peek())) {
		switch (curState) {
			case START:

				if (c == EOF) {	 // End of file
					lex = LexItem(DONE, "", linenum);
					return lex;

				} else if (isalpha(c) or c == '_') {  // Identifiers
					lexeme += in.get();
					curState = IN_IDENT;
				} else {  // FIX: temporarily skip everything else
					in.get();
				}
				break;

			case IN_IDENT:

				// TODO: use id_or_kw() to check for reserved words/keywords

				// Checks if the character is still apart of an IDENT
				// If not, return an identifier and restart
				if (!isalpha(c) && !isdigit(c) && c != '_') {
					lex = LexItem(IDENT, lexeme, linenum);
					return lex;
				}

				// Consumes the character
				lexeme += in.get();

				break;

			case IN_INT: break;

			case IN_NUM: break;

			case IN_STR: break;

			case IN_COMMENT: break;
		}
	}

	return lex;
}
