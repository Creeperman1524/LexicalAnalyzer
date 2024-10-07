// =============================================================================
// Name        : main.cpp
// Description : Tests the lexer by passing it the correct files and arguments
// =============================================================================

#include "lex.h"
#include <fstream>
#include <set>

set<string> idents;
set<string> kws;
set<float> numerics;
set<string> strings;
set<string> characters;
set<string> booleans;

map<string, Token> keyMap {
	{"if", IF},			{"else", ELSE}, {"print", PRINT},	  {"int", INT},		{"float", FLOAT},  {"char", CHAR},
	{"string", STRING}, {"bool", BOOL}, {"program", PROGRAM}, {"true", BCONST}, {"false", BCONST},
};

// Prints a set as a comma separated list
template<typename T> void printSet(set<T> set, string surround = "") {
	// Prints all elements except the last with a comma proceeding it
	for (auto item = set.begin(); item != std::prev(set.end(), 1); ++item) {
		cout << surround << *item << surround << ", ";
	}

	// Prints the last element without a comma
	cout << surround << *std::prev(set.end(), 1) << surround << endl;
}

// Loops through all keywords in order and prints them if they exist in the set
void printKeywords(set<string> set) {
	bool first = true;
	for (int i = 0; i < 11; i++) {
		for (auto word : set) {
			if (keyMap[word] == i) {
				if (first) {
					cout << word;
					first = false;
					continue;
				}
				cout << ", " << word;
			}
		}
	}

	cout << endl;
}

int main(int argc, char *argv[]) {
	// No provided file
	if (argc < 2) {
		cerr << "No specified input file." << endl;
		exit(1);
	}

	string filename = "";
	bool allFlag = false;
	bool idFlag = false;
	bool kwFlag = false;
	bool numFlag = false;
	bool strFlag = false;
	bool charFlag = false;
	bool boolFlag = false;

	// Checks the arguments for proper flags and files
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];

		if (arg.at(0) == '-') {	 // Handle flags
			if (arg == "-all")
				allFlag = true;
			else if (arg == "-id")
				idFlag = true;
			else if (arg == "-kw")
				kwFlag = true;
			else if (arg == "-num")
				numFlag = true;
			else if (arg == "-str")
				strFlag = true;
			else if (arg == "-char")
				charFlag = true;
			else if (arg == "-bool")
				boolFlag = true;
			else {
				cerr << "Unrecognized flag {" << arg << "}" << endl;
				exit(1);
			}
		} else {  // Handle files
			if (filename == "") {
				filename = arg;
			} else {
				cerr << "Only one file name is allowed." << endl;
				exit(1);
			}
		}
	}

	// Tries to open the file
	ifstream inFile;

	inFile.open(filename.c_str());
	if (!inFile) {
		// The file could not be found, so error and end the program
		cerr << "CANNOT OPEN THE FILE " << filename << endl;
		exit(1);
	}

	bool completed = false;
	bool hasText = false;
	int numLines = 1;
	int numTokens = 0;

	// Reads a file to extract all of its tokens, erroring when necessary
	while (!completed) {
		// Reads each token with lex.cpp
		LexItem lex = getNextToken(inFile, numLines);
		Token tokenType = lex.GetToken();

		// Ends the program if an error occurs, printing the error
		if (tokenType == ERR) {
			cout << lex;
			exit(1);
		}

		// Ends the loop once the entire file is read
		if (tokenType == DONE) {
			completed = true;
			break;
		} else {
			hasText = true;
			numTokens++;
		}

		// -all flag, printing each token as it is read
		if (allFlag)
			cout << lex;

		string lexeme = lex.GetLexeme();
		// Creates the list of values to be printed
		if (tokenType == IDENT)
			idents.insert(lexeme);
		else if (tokenType < 9)
			kws.insert(lexeme);
		else if (tokenType == RCONST)
			numerics.insert(std::stof(lexeme));
		else if (tokenType == ICONST)
			numerics.insert(std::stof(lexeme));
		else if (tokenType == BCONST)
			booleans.insert(lexeme);
		else if (tokenType == SCONST)
			strings.insert(lexeme);
		else if (tokenType == CCONST)
			characters.insert(lexeme);
	}

	// Detects an empty file
	if (!hasText) {
		cerr << "Empty file." << endl;
		exit(1);
	}

	// Prints the statistics
	cout << "\nLines: " << numLines << endl;
	cout << "Total Tokens: " << numTokens << endl;
	cout << "Identifiers and Keywords: " << idents.size() + kws.size() << endl;		   // IDENT and keywords
	cout << "Numerics: " << numerics.size() << endl;								   // ICONST and RCONST
	cout << "Booleans: " << booleans.size() << endl;								   // BCONST
	cout << "Strings and Characters: " << strings.size() + characters.size() << endl;  // SCONST and CCONST

	// Prints each of the lists if there are words in the list and its flag is active
	if (numFlag && !numerics.empty()) {
		cout << "NUMERIC CONSTANTS:" << endl;
		printSet(numerics);
	}
	if (boolFlag && !booleans.empty()) {
		cout << "BOOLEAN CONSTANTS:" << endl;
		printSet(booleans);
	}
	if (charFlag && !characters.empty()) {
		cout << "CHARACTER CONSTANTS:" << endl;
		printSet(characters, "\'");
	}
	if (strFlag && !strings.empty()) {
		cout << "STRINGS:" << endl;
		printSet(strings, "\"");
	}
	if (idFlag && !idents.empty()) {
		cout << "IDENTIFIERS:" << endl;
		printSet(idents);
	}
	if (kwFlag && !kws.empty()) {
		cout << "KEYWORDS:" << endl;
		printKeywords(kws);
	}

	return 0;
}
