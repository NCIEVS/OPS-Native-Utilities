#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <cstdlib>

#include "CommandLine.h"

using namespace std;

void showUsage(string prog)
{	cerr << endl << "Usage:" << endl
		<< "\t" << prog << " [-h|H][-u|U] -c -d [-e] -i -f input-file" << endl;
	return;
}
void showDetailedUsage(string prog)
{ 
	cerr << "The program takes an input delimited-file, extracts fields from specific columns, and outputs" << endl
		<< "to stdout.  If no input-file is entered, the program accepts input from stdin, and requires" << endl
		<< "^Z or EOF to process the input." << endl << endl;
	showUsage(prog); 
	cerr << endl << "where:" << endl
		<< "\t" << "u or U show simple usage" << endl
		<< "\t" << "h or H show this message" << endl 
		<< "\t" << "d flags the (mandatory) delimiter used in the delimited input file" << endl
		<< "\t" << "c flags a comma-delimited list of column numbers to \"get\" from the input file" << endl
		<< "\t" << "e flags the (optional) escape character; the '\\' character is taken as default" << endl
		<< "\t" << "i indicates to 'inspect' the first line of the file" << endl
		<< "\t" << "f flags the input filename." << endl << endl;
	return; 
}
vector<string> getFieldData (const string & line, const char delimiter, const char escapeChar);
vector<string> getFieldData ( const string & line, const string & delimiter );

int main (int argc, char * argv[])
{
	clock_t tstart, tend;
	tstart = clock();

	// DEAL WITH COMMAND LINE, ARGUMENTS, AND INPUT FILES

	// check command line
	CommandLine cmd(argc, argv);
	if( !cmd.hasAllOptionsIn("cdefi") ) {
		if( cmd.hasOption('H') || cmd.hasOption('h') ) 
			showDetailedUsage(cmd.getProgramName());
		else
			showUsage(cmd.getProgramName());
		exit(0);
		}

	// check if user only wants to inspect row data
	bool inspect = false;
	if( cmd.hasOption('i') )
		inspect = true;

	// get input delimiter for row data
	char delimiter;
	if( !inspect ) {
		if( cmd.optionFlagsArgument('d') )
			delimiter = (cmd.getFlaggedVariable('d'))[0];
		else {
			showDetailedUsage(cmd.getProgramName());
			exit(0);
			}
		}

	// get escape character for the delimiter, if none stated, use default slash for e.g. \t
	char escape;
	if( cmd.optionFlagsArgument('e') )
		escape = (cmd.getFlaggedVariable('e'))[0];
	else
		escape = '\\';

	// get column numbers to read, if no columns stated, only column 1
	set<int> col;
	if( cmd.optionFlagsArgument('c') ) {
		string strcols = cmd.getFlaggedVariable('c');
		vector<string> colstr = getFieldData(strcols, ",");
		if( colstr.size() > 0 ) {
			vector<string>::iterator pColStr = colstr.begin();
			for( ; pColStr != colstr.end(); ++pColStr )
				col.insert( atoi( (*pColStr).c_str() ) );
			}
		else
			col.insert(1);
		}

	// get input file's name
	string inputfilename = "";
	if( cmd.optionFlagsArgument('f') )
		inputfilename = cmd.getFlaggedVariable('f');


	// READ INPUT FILE, PROCESS THE ROWS, AND OUTPUT ROWS WITH EXTRACTED COLUMNS TO STDOUT

	// if there's an input filename, fetch it and bind it to cin
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
		// but if user wants only to inspect the data, read one line and output
	string inputLine;
	vector< vector<string> > term_list;
	if( inspect ) {
		getline(cin, inputLine);
		cout <<  inputLine << endl;
		}		
	else {
		while( getline(cin, inputLine), !cin.eof())
			if( inputLine.size() > 0 )
				term_list.push_back(getFieldData(inputLine, delimiter, escape));
		}
	// unbind cin if necessary and close file
	if(	inputfilename != "") {
		cin.rdbuf(backcinstrm);
		inputFile.close();
		}

	// output unsorted lines, includes duplicates
	if( !inspect ) {
		bool hasData;
		vector<string>::iterator pT;
		vector<vector<string> >::iterator pTerm = term_list.begin();
		for( ; pTerm != term_list.end(); ++pTerm) {
			pT = (*pTerm).begin();
			hasData = false;
			for( int i =1; pT != (*pTerm).end(); ++pT, ++i ) {
				if( col.find(i) != col.end() ) {
					if( hasData )
						cout << delimiter;
					hasData = true;
					cout << *pT ;
					}
				}
				if( hasData )
					cout << endl;
			}
		}

	tend = clock();
	cerr << (tend-tstart)/1000.0 << " seconds elapsed time." << endl;

	return 0;
}

vector<string> getFieldData (const string & line, const char delimiter, const char escapeChar)
{
	vector<string> tmpString;
	bool isEscaped = false;

	string::size_type start = 0, pos = 0;
	while( pos < line.size() ) {
		if ( escapeChar == line.at(pos) ) {
			isEscaped = true;
			pos++;
			}
		else {
			if( line.at(pos) != delimiter ) {					// character is not a delimiter
				if( isEscaped )
					isEscaped = false;
				pos++;
				}
			else {												// character is a delimiter
				if( isEscaped ) {								// but it is escaped
					isEscaped = false;
					pos++;
					}
				else {											// ok, delimiter is not escaped
					tmpString.push_back( line.substr( start, pos - start) );
					start = ++pos;
					}
				}
			}
		}
	if( pos != start )
		tmpString.push_back(line.substr( start, pos - start));
	return tmpString;
}

vector<string> getFieldData ( const string & line, const string & delimiters )  // basic tokenizer
{
	string::size_type start = 0, end = 0;
	string localString;
	vector<string> tmpString;
	bool endProc = false;

	while( !endProc ) {
		start = line.find_first_not_of( delimiters, end );
		if( start == string::npos ) 
			break;
		end = line.find_first_of( delimiters, start );
		if( end != string::npos) {
			tmpString.push_back( line.substr( start, end - start ) );
			}
		else { 										// ok, ran into last token
			tmpString.push_back( line.substr( start, end - start ) ); 
			endProc = true;
			}
		}
	return tmpString;
}
