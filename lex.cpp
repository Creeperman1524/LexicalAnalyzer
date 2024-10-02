// =======================================================
// Name        : lex.cpp
// Description : Takes in text to be converted to tokens
// =======================================================

#include "lex.h"

/*ostream &operator<<(ostream &out, const LexItem &tok) {}*/

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
