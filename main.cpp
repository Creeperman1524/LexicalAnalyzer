// =============================================================================
// Name        : main.cpp
// Description : Tests the lexer by passing it the correct files and arguments
// =============================================================================

#include "lex.h"
#include <fstream>
#include <iostream>
#include <sstream>
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

	bool hasText = false;
	int lines = 0;

	// Reads each line from the file
	string line;
	while (getline(inFile, line)) {
		stringstream lineStream(line);
		lines++;
		hasText = true;

		// Reads each word and performs actions against them
		LexItem token = getNextToken(lineStream, lines);

		if (token.GetToken() == ERR) {
			// TODO: make use of the operator<< function to display an error
			cerr << "Token is invalid" << endl;
			exit(1);
		}

		// TODO: print out the tokens
	}

	// Detects an empty file
	if (!hasText) {
		cerr << "Empty file." << endl;
		exit(1);
	}

	return 0;
}
