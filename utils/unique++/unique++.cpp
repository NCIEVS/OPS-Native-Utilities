#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
// #include <ctime>
#include <chrono>
#include <cstdlib>

#include "../gclib/CommandLine.h"

using namespace std;
using namespace std::chrono;

void showUsage(string prog)
{	cout << endl << "Usage:" << endl
		<< "\t" << prog << " [-h|H][-u|U] [-c] input-file" << endl;
	return;
}
void showDetailedUsage(string prog)
{ 
	showUsage(prog); 
	cout << endl << "where:" << endl
		<< "\t" << "u or U show simple usage" << endl
		<< "\t" << "h or H show this message" << endl
		<< "\t" << "c counts the unique elements" << endl << endl
		<< "The program takes an input-file, sorts the lines, and outputs to stdout." << endl
		<< "If no input-file is entered, the program accepts input from stdin, and requires" << endl
		<< "^Z or EOF to process the input." << endl;
	return; 
}

int main (int argc, char * argv[])
{
	auto tstart = system_clock::now();;

	// check command line
	CommandLine cmd(argc, argv);
	if( cmd.hasOption('h') || cmd.hasOption('H') ) {
		showDetailedUsage(cmd.getProgramName());
		exit(0);
		}
	if( cmd.hasOption('u') || cmd.hasOption('U') ) {
		showUsage(cmd.getProgramName());
		exit(0);
		}
	bool numbered = false;
	if( cmd.hasOption('c') )
		numbered = true;

	// read input from cin, but if there's an input filename, fetch it and bind it to cin
	string inputfilename = "";
	if( cmd.hasArguments() )
		inputfilename = cmd.getVariableByPosition(1);
	streambuf *backcinstrm, *infilestrm;
	ifstream inputFile;
	if( inputfilename != "") {
		inputFile.open(inputfilename.c_str(), ios::in);
		if (inputFile.bad()) {
			cerr << "Can't open \"" << inputfilename.c_str() << ".\"  Will EXIT." << endl;
			showUsage(cmd.getProgramName());
			exit(0);
			}
		backcinstrm = cin.rdbuf();
		infilestrm = inputFile.rdbuf();
		cin.rdbuf(infilestrm);
		}

	// read input
	map<string,long> term_count;
	set<string> term_list;
	string inputLine;
	if( numbered ) {
		while( getline(cin, inputLine), !cin.eof() ) {
			if( term_count.find(inputLine) == term_count.end() )
				term_count[inputLine] = 1;
			else 
				term_count[inputLine]++;
			}
		}
	else {
		while( getline(cin, inputLine), !cin.eof() )
			term_list.insert(inputLine);
		}

	// unbind cin if necessary
	if(	inputfilename != "") {
		cin.rdbuf(backcinstrm);
		inputFile.close();
		}

	// output sorted lines without duplicates, or preceeded by counts
	if( numbered ) {
		map<string,long>::iterator pTerm = term_count.begin();
		for( ; pTerm != term_count.end(); ++pTerm ) {
			cout << pTerm->second << "\t" << pTerm->first << endl;
			}
		}
	else {
		set<string>::iterator pTerm = term_list.begin();
		for( ; pTerm != term_list.end(); ++pTerm ) 
			cout << *pTerm << endl;
		}

	auto tend = system_clock::now();
	duration<double> diff = tend - tstart;
	cerr <<  std::fixed << std::setprecision(3) << diff.count() << " seconds wall clock elapsed time." << endl;

	return 0;
}
