// =============================================================================
// Name        : main.cpp
// Description : Tests the lexer by passing it the correct files and arguments
// =============================================================================

#include "lex.h"
#include <algorithm>
#include <fstream>
#include <vector>

vector<string> idents;
vector<string> kws;
vector<string> numerics;
vector<string> strings;
vector<string> characters;
vector<string> booleans;

map<string, Token> keyMap {
	{"if", IF},			{"else", ELSE}, {"print", PRINT},	  {"int", INT},		{"float", FLOAT},  {"char", CHAR},
	{"string", STRING}, {"bool", BOOL}, {"program", PROGRAM}, {"true", BCONST}, {"false", BCONST},
};

void printVec(vector<string> vec, string surround = "") {
	for (int i = 0; i < vec.size() - 1; i++) { cout << surround << vec.at(i) << surround << ", "; }

	cout << surround << vec.at(vec.size() - 1) << surround << endl;
}

// Inserts while checking for duplicates
void insertIntoVec(vector<string> &vec, string lex) {
	for (string &l : vec) {
		if (l == lex) {
			return;
		}
	}

	vec.push_back(lex);
}

void sortKeywords(vector<string> &vec) {
	int n = vec.size();

	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (keyMap[vec[j]] > keyMap[vec[j + 1]]) {
				string temp = vec[j];
				vec[j] = vec[j + 1];
				vec[j + 1] = temp;
			}
		}
	}
}

void sortNumerics(vector<string> &vec) {
	// Pairs together the int/float + string
	vector<tuple<float, string>> pairs;
	for (string i : vec) { pairs.push_back(make_tuple(stof(i), i)); }

	// Sorts the pair array
	int n = pairs.size();

	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (get<0>(pairs[j]) > get<0>(pairs[j + 1])) {
				tuple<float, string> temp = pairs[j];
				pairs[j] = pairs[j + 1];
				pairs[j + 1] = temp;
			}
		}
	}

	// Copies the sorted values onto the vector
	for (int i = 0; i < pairs.size(); i++) { vec[i] = get<1>(pairs[i]); }
}

string convertStringInt(string num) {
	string result = "";

	// Gets rid of leading +
	for (char &c : num) {
		if (c != '+')
			result += c;
	}

	return result;
}

string convertStringFloat(string num) {
	string result = "";
	bool sawOp = false;

	// Adds a leading 0 when it sees a .
	for (char &c : num) {
		if (c == '.' && sawOp) {
			result += "0.";
			sawOp = false;
		} else if (c == '-') {
			result += c;
			sawOp = true;
		} else if (c == '+') {
			sawOp = true;
		} else {
			result += c;
			sawOp = false;
		}
	}

	return result;
}

int main(int argc, char *argv[]) {
	// No provided file
	if (argc < 2) {
		cerr << "No specified input file." << endl;
		exit(1);
	}

	string filename = "";
	bool noFlags = true;
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
			if (arg == "-all") {
				allFlag = true;
			} else if (arg == "-id") {
				idFlag = true;
				noFlags = false;
			} else if (arg == "-kw") {
				kwFlag = true;
				noFlags = false;
			} else if (arg == "-num") {
				numFlag = true;
				noFlags = false;
			} else if (arg == "-str") {
				strFlag = true;
				noFlags = false;
			} else if (arg == "-char") {
				charFlag = true;
				noFlags = false;
			} else if (arg == "-bool") {
				boolFlag = true;
				noFlags = false;
			} else {
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
			insertIntoVec(idents, lex.GetLexeme());
		} else if (tokenType < 9) {
			insertIntoVec(kws, lex.GetLexeme());
		} else if (tokenType == RCONST) {  // Integers and Real Constants
			insertIntoVec(numerics, convertStringFloat(lex.GetLexeme()));
		} else if (tokenType == ICONST) {
			insertIntoVec(numerics, convertStringInt(lex.GetLexeme()));
		} else if (tokenType == BCONST) {  // Booleans
			insertIntoVec(booleans, lex.GetLexeme());
		} else if (tokenType == SCONST) {  // Strings
			insertIntoVec(strings, lex.GetLexeme());
		} else if (tokenType == CCONST) {  // Characters
			insertIntoVec(characters, lex.GetLexeme());
		}
	}

	// Detects an empty file
	if (!hasText) {
		cerr << "Empty file." << endl;
		exit(1);
	}

	cout << "\nLines: " << numLines << endl;
	cout << "Total Tokens: " << numTokens << endl;								 // not counting done
	cout << "Identifiers and Keywords: " << idents.size() + kws.size() << endl;	 // IDENT and keywords
	cout << "Numerics: " << numerics.size() << endl;							 // ICONST and RCONST
	cout << "Booleans: " << booleans.size() << endl;							 // BCONST
	cout << "Strings and Characters: " << strings.size() + characters.size() << (allFlag && noFlags ? "\n" : "")
		 << endl;  // SCONST and CCONST

	sortNumerics(numerics);
	std::sort(booleans.begin(), booleans.end());
	std::sort(characters.begin(), characters.end());
	std::sort(strings.begin(), strings.end());
	std::sort(idents.begin(), idents.end());
	sortKeywords(kws);

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
	if (kwFlag && !kws.empty()) {
		cout << "KEYWORDS:" << endl;
		printVec(kws);
	}

	return 0;
}
