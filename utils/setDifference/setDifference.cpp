//
#include <set>
#include <algorithm>
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
		<< "\t" << prog << " -h|H -s -1 file1 -2 file2" << endl;
	return;
}
void showDetailedUsage(string prog)
{ 
	showUsage(prog); 
	cout << endl << "where:" << endl
		<< "\t" << "-h or -H show this message" << endl 
		<< "\t" << "-1 flags a baseline comparison file" << endl
		<< "\t" << "-2 flags a file to compare against the baseline" << endl
		<< "\t" << "-s flags for a set-symmetric-difference instead of the default set-difference" << endl << endl
		<< "The program takes input file1 and file2, and outputs a set-difference to stdout."  << endl
		<< "The input files contain \"terms\" to compare (entire input lines).  In the set-"  << endl
		<< "difference, anything in file1 not present in file2 is output.  If the set-symmetric" << endl
		<< "difference is selected (-s), the unique elements in both file1 and file2 are output" << endl
		<< "instead.  That is, " << endl
		<< "\t" << "set-diff(s1,s2) = s1 - s2" << endl
		<< "\t" << "set-symmetric-diff(s1,s2) = (s1 union s2) - (s1 intersection s2)" << endl 
		<< endl;
	return; 
}

int main(int argc, char *argv[])
{
	// READ COMMAND LINE
	CommandLine cmd(argc, argv);
	if( !cmd.hasAllOptionsIn("12s") || !cmd.hasArguments() || !cmd.hasMandatoryOptions("12") ) {
		if( cmd.hasOption('H') || cmd.hasOption('h') ) 
			showDetailedUsage(cmd.getProgramName());
		else
			showUsage(cmd.getProgramName());
		exit(0);
		}

	// CHECK INPUT FILES
	string inputfilename = "";
	if( cmd.optionFlagsArgument('1') )
		inputfilename = cmd.getFlaggedVariable('1');
	ifstream inputFile;
	inputFile.open(inputfilename.c_str());
	if (inputFile.bad()) {
		cerr << "Can't open \"" << inputfilename.c_str() << ".\"  Will EXIT." << endl;
		showUsage(cmd.getProgramName());
		exit(0);
		}

	string inputfilename2 = "";
	if( cmd.optionFlagsArgument('2') )
		inputfilename2 = cmd.getFlaggedVariable('2');
	ifstream inFile2;
	inFile2.open(inputfilename2.c_str());
	if (inFile2.bad()) {
		cerr << "Can't open \"" << inputfilename2 << ".\"  Will EXIT." << endl;
		showUsage(cmd.getProgramName());
		exit(0);
		}

	// READ FILES
	string inputLine;
	set<string> fileset1, fileset2, outset;

	while( getline(inputFile, inputLine), !inputFile.eof() )
		fileset1.insert(inputLine);
	inputFile.close();
	while( getline(inFile2, inputLine), !inFile2.eof() )
		fileset2.insert(inputLine);
	inFile2.close();

	
	// COMPUTE SET DIFFERENCE
	if( cmd.hasOption('s') )
		set_symmetric_difference( fileset1.begin(), fileset1.end(), fileset2.begin(), fileset2.end(), 
			insert_iterator< set<string> > (outset, outset.begin()) );
	else
		set_difference( fileset1.begin(), fileset1.end(), fileset2.begin(), fileset2.end(), 
			insert_iterator< set<string> > (outset, outset.begin()) );
		
	// OUTPUT SET DIFFERENCE
	set<string>::iterator pOut = outset.begin();
	for( ; pOut != outset.end(); ++pOut )
		cout << *pOut << endl;


	return 0;
}


