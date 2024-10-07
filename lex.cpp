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

map<string, Token> stringToEnum {
	{"if", IF},			{"else", ELSE}, {"print", PRINT},	  {"int", INT},		{"float", FLOAT},  {"char", CHAR},
	{"string", STRING}, {"bool", BOOL}, {"program", PROGRAM}, {"true", BCONST}, {"false", BCONST},
};

string opsAndDelims = "+-*/=!<>%&|,;(){}.";

// Overloads the << operator to print out LexItems properly
ostream &operator<<(ostream &out, const LexItem &tok) {
	Token token = tok.GetToken();

	// Output the elements as strings rather than ints
	out << enumToString[token];

	// Adds special formatting for these tokens
	if (token == ICONST || token == RCONST || token == BCONST)
		out << ": (" << tok.GetLexeme() << ")";
	else if (token == IDENT)
		out << ": <" << tok.GetLexeme() << ">";
	else if (token == SCONST)
		out << ": \"" << tok.GetLexeme() << "\"";
	else if (token == CCONST)
		out << ": \'" << tok.GetLexeme() << "\'";
	else if (token == ERR)
		out << ": In line " << tok.GetLinenum() << ", Error Message {" << tok.GetLexeme() << "}\n";

	out << endl;
	return out;
}

// Returns a LexItem of a keyword or identifier, depending on which it is
LexItem id_or_kw(const string &lexeme, int linenum) {
	string lowerLexeme = "";

	// Converts the name to lowercase to void case sensitivity
	for (char c : lexeme) lowerLexeme += tolower(c);

	// Checks if the keyword exists
	if (stringToEnum.find(lowerLexeme) != stringToEnum.end())
		return LexItem(stringToEnum[lowerLexeme], lowerLexeme, linenum);

	// No keyword exists, so return an identifier
	return LexItem(IDENT, lexeme, linenum);
}

// Gets the next token from a stream
LexItem getNextToken(istream &in, int &linenum) {
	// States of the lexer, used to determine multi-character tokens
	enum STATES { START, IN_IDENT, IN_INT, IN_NUM, IN_OP, IN_STR, IN_CHAR, MULTI_COMMENT, SINGLE_COMMENT };

	// Continually consumes characters to find lexemes
	string lexeme;
	char c;
	STATES curState = START;

	while ((c = in.peek())) {
		switch (curState) {
			case START:

				if (c == EOF) {	 // End of file
					return LexItem(DONE, "", linenum);

				} else if (c == '\n') {	 // New lines

					// FIX: This is actually hot garbage but it works :)
					in.get();
					while (in.peek() == ' ') in.get();	// Consumes all the whitespace

					c = in.peek();
					if (c == EOF) continue;
					if (c == '\n') {
						in.get();
						if (in.peek() == EOF) continue;
						linenum++;
					}

					linenum++;

				} else if (opsAndDelims.find(c) != string::npos) {	// Operators and Delimiters
					lexeme += in.get();
					curState = IN_OP;

				} else if (isalpha(c) or c == '_') {  // Identifiers
					lexeme += in.get();
					curState = IN_IDENT;

				} else if (isdigit(c)) {  // Integers and Real Constants
					lexeme += in.get();
					curState = IN_INT;

				} else if (c == '\"') {	 // Strings
					in.get();			 // Consumes the starting "
					curState = IN_STR;

				} else if (c == '\'') {	 // Characters
					in.get();			 // Consumes the starting '
					curState = IN_CHAR;

				} else if (isspace(c)) {
					in.get();  // Ignore all excess whitespace

				} else {
					lexeme += in.get();
					return LexItem(ERR, lexeme, linenum);
				}
				break;

			case IN_IDENT:

				// Checks if the character is still apart of an IDENT
				// If not, return an identifier and restart
				if (!isalpha(c) && !isdigit(c) && c != '_') return id_or_kw(lexeme, linenum);

				// Adds the character to the lexeme
				lexeme += in.get();
				break;

			case IN_INT:  // Integer constants

				// End of the integer
				if (!isdigit(c) && c != '.') return LexItem(ICONST, lexeme, linenum);

				// Check for possible Real Constant
				if (c == '.') {
					lexeme += in.get();
					if (isdigit(in.peek())) {
						curState = IN_NUM;
						continue;
					}

					// ICONST followed by DOT (5.)
					// Puts back the dot we used to check the character following it
					lexeme.pop_back();
					in.putback('.');
					return LexItem(ICONST, lexeme, linenum);
				}

				lexeme += in.get();
				break;

			case IN_NUM:  // Real constants

				// Error (1.7.2)
				if (c == '.') return LexItem(ERR, lexeme + '.', linenum);

				// End of the Real Constant
				if (!isdigit(c)) return LexItem(RCONST, lexeme, linenum);

				lexeme += in.get();
				break;

			case IN_OP:	 // Operators and delimiters

				switch (lexeme.at(0)) {
					case '+':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							return LexItem(ADDASSOP, lexeme, linenum);
						} else if (isdigit(c)) {  // Start of an Integer Constant
							lexeme += in.get();
							curState = IN_INT;
							continue;
						} else if (c == '.') {	// Start of a Real Constant
							lexeme += in.get();
							curState = IN_NUM;
							continue;
						}

						// Single character
						return LexItem(PLUS, lexeme, linenum);
						break;
					case '-':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							return LexItem(SUBASSOP, lexeme, linenum);
						} else if (isdigit(c)) {  // Start of an Integer Constant
							lexeme += in.get();
							curState = IN_INT;
							continue;
						} else if (c == '.') {	// Start of a Real Constant
							lexeme += in.get();
							curState = IN_NUM;
							continue;
						}

						// Single character
						return LexItem(MINUS, lexeme, linenum);
						break;
					case '*':

						// Check for assignment operation
						if (in.peek() == '=') {
							lexeme += in.get();
							return LexItem(MULASSOP, lexeme, linenum);
						}

						// Single character
						return LexItem(MULT, lexeme, linenum);
						break;
					case '/':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							return LexItem(DIVASSOP, lexeme, linenum);
						} else if (c == '/') {	// Single-comment
							lexeme.pop_back();	// Takes the / out of the lexeme
							in.get();
							curState = SINGLE_COMMENT;
							continue;
						} else if (c == '*') {	// Multi-Comment
							lexeme.pop_back();	// Takes the / out of the lexeme
							in.get();
							curState = MULTI_COMMENT;
							continue;
						}

						// Single character
						return LexItem(DIV, lexeme, linenum);
						break;
					case '=':

						// Check for assignment operation
						if (in.peek() == '=') {
							lexeme += in.get();
							return LexItem(EQ, lexeme, linenum);
						}

						// Single character
						return LexItem(ASSOP, lexeme, linenum);
						break;
					case '!':

						// Check for assignment operation
						if (in.peek() == '=') {
							lexeme += in.get();
							return LexItem(NEQ, lexeme, linenum);
						}

						// Single character
						return LexItem(NOT, lexeme, linenum);
						break;
					case '<': return LexItem(LTHAN, lexeme, linenum); break;
					case '>': return LexItem(GTHAN, lexeme, linenum); break;
					case '%':

						// Check for assignment operation
						if (in.peek() == '=') {
							lexeme += in.get();
							return LexItem(REMASSOP, lexeme, linenum);
						}

						// Single character
						return LexItem(REM, lexeme, linenum);
						break;
					case '&':

						// Check for the double character
						if (in.peek() == '&') {
							lexeme += in.get();
							return LexItem(AND, lexeme, linenum);
						}

						// Error for single &
						return LexItem(ERR, lexeme, linenum);
						break;
					case '|':

						// Check for the double character
						if (in.peek() == '|') {
							lexeme += in.get();
							return LexItem(OR, lexeme, linenum);
						}

						// Error for single |
						return LexItem(ERR, lexeme, linenum);
						break;
					case ',': return LexItem(COMMA, lexeme, linenum); break;
					case ';': return LexItem(SEMICOL, lexeme, linenum); break;
					case '(': return LexItem(LPAREN, lexeme, linenum); break;
					case ')': return LexItem(RPAREN, lexeme, linenum); break;
					case '{': return LexItem(LBRACE, lexeme, linenum); break;
					case '}': return LexItem(RBRACE, lexeme, linenum); break;
					case '.':

						// Checks if it's the start of a Real Constant
						if (isdigit(in.peek())) {
							lexeme += in.get();
							curState = IN_NUM;
							continue;
						}
						return LexItem(DOT, lexeme, linenum);
						break;
				}

				break;

			case IN_STR:  // String constants

				// Correct string
				if (c == '\"') {
					in.get();  // Consumes the final delimiter
					return LexItem(SCONST, lexeme, linenum);
				}

				// Mismatched pair
				if (c == '\'') return LexItem(ERR, " Invalid string constant \"" + lexeme + "\'", linenum);

				// Missing pair
				if (c == '\n') return LexItem(ERR, " Invalid string constant \"" + lexeme, linenum);

				lexeme += in.get();	 // Adds all characters into the string constant
				break;

			case IN_CHAR:  // Character constants

				// Correct char
				if (c == '\'') {
					in.get();  // Consumes the final delimiter
					return LexItem(CCONST, lexeme, linenum);
				}

				// Too long
				if (lexeme.length() > 1) return LexItem(ERR, " Invalid character constant \'" + lexeme + "\'", linenum);

				// Missing pair
				if (c == '\n') return LexItem(ERR, "New line is an invalid character constant.", linenum);

				lexeme += in.get();	 // Adds all characters into the character constant
				break;

			case MULTI_COMMENT:	 // Multi-line comment

				// Keep the count of lines
				if (c == '\n') linenum++;

				// Keeps checking for */
				if (c == '*') {
					in.get();
					if (in.peek() == '/') curState = START;
				}

				in.get();  // Consumes all the characters in the comment

				break;

			case SINGLE_COMMENT:  // Single-line comment

				// Keeps checking for a new line
				if (c == '\n') {
					linenum++;
					curState = START;
				}

				in.get();  // Consumes all the characters in the comment

				break;
		}
	}

	return LexItem();
}
