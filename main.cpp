// =============================================================================
// Name        : main.cpp
// Description : Tests the lexer by passing it the correct files and arguments
// =============================================================================

#include "lex.h"
#include <fstream>
#include <vector>

vector<LexItem> idents;
vector<LexItem> keywords;
vector<LexItem> numerics;
vector<LexItem> strings;
vector<LexItem> characters;
vector<LexItem> booleans;

void printVec(vector<LexItem> vec, string surround = "") {
	for (int i = 0; i < vec.size() - 1; i++) { cout << surround << vec.at(i).GetLexeme() << surround << ", "; }

	cout << surround << vec.at(vec.size() - 1).GetLexeme() << surround << endl;
}

// Inserts while checking for duplicates
void insertIntoVec(vector<LexItem> &vec, LexItem lex) {
	for (LexItem &l : vec) {
		if (l.GetLexeme() == lex.GetLexeme()) {
			return;
		}
	}

	vec.push_back(lex);
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

	while (!completed) {
		// Reads each token
		LexItem lex = getNextToken(inFile, numLines);
		Token tokenType = lex.GetToken();

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

		if (allFlag)
			cout << lex;

		// Creates the list of values to be printed
		if (tokenType == IDENT) {
			insertIntoVec(idents, lex);
		} else if (tokenType < 9) {
			insertIntoVec(keywords, lex);
		} else if (tokenType == RCONST || tokenType == ICONST) {  // Integers and Real Constants
			insertIntoVec(numerics, lex);
		} else if (tokenType == BCONST) {  // Booleans
			insertIntoVec(booleans, lex);
		} else if (tokenType == SCONST) {  // Strings
			insertIntoVec(strings, lex);
		} else if (tokenType == CCONST) {  // Characters
			insertIntoVec(characters, lex);
		}
	}

	// Detects an empty file
	if (!hasText) {
		cerr << "Empty file." << endl;
		exit(1);
	}

	cout << "\nLines: " << numLines << endl;
	cout << "Total Tokens: " << numTokens << endl;									   // not counting done
	cout << "Identifiers and Keywords: " << idents.size() + keywords.size() << endl;   // IDENT and keywords
	cout << "Numerics: " << numerics.size() << endl;								   // ICONST and RCONST
	cout << "Booleans: " << booleans.size() << endl;								   // BCONST
	cout << "Strings and Characters: " << strings.size() + characters.size() << endl;  // SCONST and CCONST

	if (numFlag && !numerics.empty()) {
		cout << "NUMERIC CONSTANTS:" << endl;
		printVec(numerics);
	}
	if (boolFlag && !booleans.empty()) {
		cout << "BOOLEAN CONSTANTS:" << endl;
		printVec(booleans);
	}
	if (charFlag && !characters.empty()) {
		cout << "CHARACTER CONSTANTS:" << endl;
		printVec(characters, "\'");
	}
	if (strFlag && !strings.empty()) {
		cout << "STRINGS:" << endl;
		printVec(strings, "\"");
	}
	if (idFlag && !idents.empty()) {
		cout << "IDENTIFIERS:" << endl;
		printVec(idents);
	}
	if (kwFlag && !keywords.empty()) {
		cout << "KEYWORDS:" << endl;
		printVec(keywords);
	}

	return 0;
}
