// examines non-ascii characters and reports them with different options
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
{
	cout << endl << "Usage:" << endl
		<< "\t" << prog << " -[H|h][l][b][t][c] -f input-file" << endl;
	return;
}

void showDetailedUsage(string prog)
{ 
	showUsage(prog); 
	cout << endl << "where:" << endl
		<< "\t" << "h or H: display this message" << endl 
		<< "\t" << "l:  outputs line numbers" << endl 
		<< "\t" << "b:  outputs position in file in bytes" << endl
		<< "\t" << "t:  outputs text up to the position in the line where character is found" << endl
		<< "\t" << "c:  outputs column position, subsequent chars will be increasingly offset" << endl
		<< "\t" << "f:  flag denotes the input-file to analyze" << endl
		<< endl
		<< "The program takes an input-file, examines characters, and outputs to stdout a report of the" << endl
		<< " characters with the high bit set.  If no input-file is entered, the program accepts input from stdin" << endl
		<< "(e.g. piped or redirected), and requires a terminating EOF (or ^Z if entering text from keyboard)" << endl
		<< "to process the input." << endl;
	return;
}

int main(int argc, char *argv[])
{
	CommandLine cmd(argc, argv);
	if( !cmd.hasAllOptionsIn("flbtc") ) {
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
		inputFile.open(inputfilename.c_str(), ios::binary | ios::in);
		if (inputFile.bad()) {
			cerr << "Can't open \"" << inputfilename.c_str() << ".\"  Will EXIT." << endl;
			exit(0);
			}
		backcinstrm = cin.rdbuf();
		infilestrm = inputFile.rdbuf();
		cin.rdbuf(infilestrm);
		}
	

	bool alreadyFound = false;
	char tmpChar = 0, lstChar = 0;
	long filePos = 0;
	long startPos = 0;
	long lineNumber = 1;
	short mask = 0x00ff;
	vector<char> mStrbuffer;
	vector<char> linebuffer;

	while( !cin.eof() ) {
		lstChar = tmpChar;
		cin.get(tmpChar);
		linebuffer.push_back(tmpChar);
		filePos++;
		startPos++;
		if( tmpChar < 0 ) {
			mStrbuffer.push_back(tmpChar);
			if( !alreadyFound ) {
				cout << "! Found encoded character " << endl;
				alreadyFound = true;
				}
			if( lstChar > 0 ) {
				if( cmd.hasOption('l') )
					cout << '\t' << "line " << lineNumber << "; ";
				if( cmd.hasOption('b') )
					cout << "byte "<< filePos << "; ";
				if( cmd.hasOption('c') )
					cout << "col " << startPos << "; ";

				}
			}
		else if( lstChar < 0 ) { 		// i.e. tmpChar > 0
			for(unsigned i = 0; i < mStrbuffer.size(); i++) 
				cout << mStrbuffer[i];
			cout << "  " << hex ;
			for(unsigned i = 0; i < mStrbuffer.size(); i++) 
				cout << ' ' << ((static_cast<unsigned short> (mStrbuffer[i])) & mask);
			cout << dec ;
			if( cmd.hasOption('t') ) {
				cout << ":  " ;
				for(unsigned i = 0; i < linebuffer.size(); i++) 
					cout << linebuffer[i]; 
				}
			cout << endl;  
			mStrbuffer.clear();
			}			
		if( tmpChar == 10 ) { // counting LF only
			lineNumber++;
			startPos = 0;
			linebuffer.clear();
			}
		}

	cout << "! Read " << filePos << " bytes in " << lineNumber << " lines." << endl;

	if(	inputfilename != "") {
		cin.rdbuf(backcinstrm);
		inputFile.close();
		}
	return 0;
}

// *****************************************************************

