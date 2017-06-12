// haven't done anything yet,
// what I want to do is strip any character at the binary level, targets are
// control characters
//

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>				// exit()

#include "CommandLine.h"

using namespace std;


void showUsage(string prog)
{	cout << endl << "Usage:" << endl
		<< "\t" << prog << " [-h|H] -f input-file -o output-file" << endl;
	return;
}
void showDetailedUsage(string prog)
{ 
	showUsage(prog); 
	cout << endl << "where:" << endl
		<< "\t" << "h or H show this message" << endl << endl
		<< "The program takes an input-file, strips non-ascii characters, and outputs to output-file." << endl
		<< "Control characters are also stripped, except for CR, LF, and TAB.  If no input-file is entered," << endl
		<< "the program accepts input from stdin, and requires ^Z or EOF to process the input.  If no" << endl
		<< "output-file is named, output is to stdout." << endl;
	return; 
}

int main(int argc, char *argv[])
{
	CommandLine cmd(argc, argv);
	if( !cmd.hasAllOptionsIn("fo") ) {
		if( cmd.hasOption('H') || cmd.hasOption('h') ) 
			showDetailedUsage(cmd.getProgramName());
		else
			showUsage(cmd.getProgramName());
		exit(0);
		}

	string inputfilename = "";
	if( cmd.optionFlagsArgument('f') )
		inputfilename = cmd.getFlaggedVariable('f');
	streambuf *backcinstrm, *infilestrm;
	ifstream inputFile;
	if( inputfilename != "") {
		inputFile.open(inputfilename.c_str(), ios::in | ios::binary );
		if (inputFile.bad()) {
			cerr << "Can't open \"" << inputfilename.c_str() << ".\"  Will EXIT." << endl;
			showUsage(cmd.getProgramName());
			exit(0);
			}
		backcinstrm = cin.rdbuf();
		infilestrm = inputFile.rdbuf();
		cin.rdbuf(infilestrm);
		}


	string outputfilename = "";
	if( cmd.optionFlagsArgument('o') )
		outputfilename = cmd.getFlaggedVariable('o');
	streambuf *backcoutstrm, *outfilestrm;
	ofstream outputFile;
	if( outputfilename != "" ) {
		outputFile.open(outputfilename.c_str());
		if (outputFile.bad()) {
			cerr << "Can't open \"" << outputfilename << ".\"  Will EXIT." << endl;
			showUsage(cmd.getProgramName());
			exit(0);
			}
		backcoutstrm = cout.rdbuf();
		outfilestrm = outputFile.rdbuf();
		cout.rdbuf(outfilestrm);
		}

	char tmpChar;
	long lineNumber = 1;
	while( !cin.eof() ) {
		cin.get(tmpChar);
		if( (tmpChar >= 32 && tmpChar <= 127) || tmpChar == 10 || tmpChar == 13 || tmpChar == 9 ) {
			cout.put(tmpChar);
			if( tmpChar == 10 )
				lineNumber++;
			}
		}

	if(	inputfilename != "") {
		cin.rdbuf(backcinstrm);
		inputFile.close();
		}
	if(	outputfilename != "") {
		cout.rdbuf(backcoutstrm);
		outputFile.close();
		}
	return 0;
}


