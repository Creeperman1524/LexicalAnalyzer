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

map<string, Token> keywords {
	{"if", IF},			{"else", ELSE}, {"print", PRINT},	  {"int", INT},		{"float", FLOAT},  {"char", CHAR},
	{"string", STRING}, {"bool", BOOL}, {"program", PROGRAM}, {"true", BCONST}, {"false", BCONST},
};

string opsAndDelims = "+-*/=!<>%&|,;(){}.";

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
		out << ": In line " << tok.GetLinenum() << ", Error Message {" << tok.GetLexeme() << "}\n";
	}

	out << endl;
	return out;
}

LexItem id_or_kw(const string &lexeme, int linenum) {
	string lowerLexeme = "";

	// Converts the name to lowercase to void case sensitivity
	for (char c : lexeme) { lowerLexeme += tolower(c); }

	// Checks if the keyword exists
	if (keywords.find(lowerLexeme) != keywords.end()) {
		return LexItem(keywords[lowerLexeme], lowerLexeme, linenum);
	}

	return LexItem(IDENT, lexeme, linenum);
}

LexItem getNextToken(istream &in, int &linenum) {
	LexItem lex = LexItem();

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
					lex = LexItem(DONE, "", linenum);
					return lex;

				} else if (c == '\n') {	 // New lines
					in.get();
					// Do not count the EOF as aa new line

					while (in.peek() == ' ') { in.get(); }	// Consumes all the whitespace

					c = in.peek();
					if (c == EOF) {
						continue;
					}
					if (c == '\n') {
						in.get();
						c = in.peek();
						if (c == EOF) {
							continue;
						}
						linenum++;
					}

					if (c != EOF) {
						linenum++;
					}

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
					in.get();			 // Consumes the "
					curState = IN_STR;

				} else if (c == '\'') {	 // Characters
					in.get();			 // Consumes the '
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
				if (!isalpha(c) && !isdigit(c) && c != '_') {
					return id_or_kw(lexeme, linenum);
				}

				// Consumes the character
				lexeme += in.get();

				break;

			case IN_INT:

				// End of the integer
				if (!isdigit(c) && c != '.') {
					lex = LexItem(ICONST, lexeme, linenum);
					return lex;
				}

				// Check for possible Real Constant
				if (c == '.') {
					lexeme += in.get();
					c = in.peek();
					if (isdigit(c)) {
						curState = IN_NUM;
						continue;
					}

					// ICONST followed by DOT
					// 5.
					// Puts back the dot we used to check the character following it
					lexeme.pop_back();
					in.putback('.');
					lex = LexItem(ICONST, lexeme, linenum);
					return lex;
				}

				lexeme += in.get();

				break;

			case IN_NUM:

				// Error
				// 1.7.2
				if (c == '.') {
					lex = LexItem(ERR, lexeme + '.', linenum);
					return lex;
				}

				// End of the Real Constant
				if (!isdigit(c)) {
					lex = LexItem(RCONST, lexeme, linenum);
					return lex;
				}

				lexeme += in.get();

				break;

			case IN_OP:

				switch (lexeme.at(0)) {
					case '+':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(ADDASSOP, lexeme, linenum);
							return lex;
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
							lex = LexItem(SUBASSOP, lexeme, linenum);
							return lex;
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
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(MULASSOP, lexeme, linenum);
							return lex;
						}

						// Single character
						return LexItem(MULT, lexeme, linenum);

						break;
					case '/':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(DIVASSOP, lexeme, linenum);
							return lex;
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
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(EQ, lexeme, linenum);
							return lex;
						}

						// Single character
						return LexItem(ASSOP, lexeme, linenum);

						break;
					case '!':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(NEQ, lexeme, linenum);
							return lex;
						}

						// Single character
						return LexItem(NOT, lexeme, linenum);

						break;
					case '<': return LexItem(LTHAN, lexeme, linenum); break;
					case '>': return LexItem(GTHAN, lexeme, linenum); break;
					case '%':

						// Check for assignment operation
						c = in.peek();
						if (c == '=') {
							lexeme += in.get();
							lex = LexItem(REMASSOP, lexeme, linenum);
							return lex;
						}

						// Single character
						return LexItem(REM, lexeme, linenum);

						break;
					case '&':

						// Check for the double character
						c = in.peek();
						if (c == '&') {
							lexeme += in.get();
							lex = LexItem(AND, lexeme, linenum);
							return lex;
						}

						// Error for single &
						return LexItem(ERR, lexeme, linenum);

						break;
					case '|':

						// Check for the double character
						c = in.peek();
						if (c == '|') {
							lexeme += in.get();
							lex = LexItem(OR, lexeme, linenum);
							return lex;
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
						c = in.peek();
						if (isdigit(c)) {
							lexeme += in.get();
							curState = IN_NUM;
							continue;
						}
						return LexItem(DOT, lexeme, linenum);
						break;
				}

				return LexItem(IDENT, "womp", linenum);

				break;

			case IN_STR:  // String constants

				// Correct string
				if (c == '\"') {
					in.get();  // Consumes the final delimiter
					lex = LexItem(SCONST, lexeme, linenum);
					return lex;
				}

				// Mismatched pair
				if (c == '\'') {
					lex = LexItem(ERR, " Invalid string constant \"" + lexeme + "\'", linenum);
					return lex;
				}

				// Missing pair
				if (c == '\n') {
					lex = LexItem(ERR, " Invalid string constant \"" + lexeme, linenum);
					return lex;
				}

				lexeme += in.get();

				break;

			case IN_CHAR:  // Character constants

				// Correct char
				if (c == '\'') {
					in.get();  // Consumes the final delimiter
					lex = LexItem(CCONST, lexeme, linenum);
					return lex;
				}

				// Too long
				if (lexeme.length() > 1) {
					lex = LexItem(ERR, " Invalid character constant \'" + lexeme + "\'", linenum);
					return lex;
				}

				// Missing pair
				if (c == '\n') {
					lex = LexItem(ERR, "New line is an invalid character constant.", linenum);
					return lex;
				}

				lexeme += in.get();

				break;

			case MULTI_COMMENT:	 // Multi-line comment

				// Keep the count of lines
				if (c == '\n') {
					linenum++;
				}

				// Keeps checking for */
				if (c == '*') {
					in.get();
					if (in.peek() == '/') {
						curState = START;
					}
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

	return lex;
}
