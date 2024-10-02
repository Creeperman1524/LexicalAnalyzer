// =============================================================================
// Name        : main.cpp
// Description : Tests the lexer by passing it the correct files and arguments
// =============================================================================

#include "lex.h"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
	// No provided file
	if (argc < 2) {
		cerr << "No specified input file" << endl;
		exit(1);
	}

	string filename = "";

	// Checks the arguments for proper flags and files
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];

		if (arg.at(0) == '-') {	 // Handle flags
			if (arg == "-all") {
			} else {
				cerr << "Unrecognized flag {" << arg << "}" << endl;
				exit(1);
			}
		} else {  // Handle files
			if (filename == "") {
				filename = arg;
			} else {
				cerr << "Only one filename is allowed." << endl;
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
	int numLines = 0;

	while (!completed) {
		// Reads each token
		LexItem token = getNextToken(inFile, numLines);

		if (token.GetToken() == ERR) {
			// TODO: make use of the operator<< function to display an error
			cerr << "Token is invalid" << endl;
			exit(1);
		}

		// Ends the loop once the entire file is read
		if (token.GetToken() == DONE) {
			completed = true;
			break;
		} else {
			hasText = true;
		}

		// TODO: print out the tokens with the operator<< overload
		cout << token.GetToken() << " " << token.GetLexeme() << " " << token.GetLinenum() << endl;
	}

	// Detects an empty file
	if (!hasText) {
		cerr << "Empty file." << endl;
		exit(1);
	}

	// TODO: print out the flag stuff

	return 0;
}
