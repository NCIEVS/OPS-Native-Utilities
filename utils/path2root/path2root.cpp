#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <cstdlib>

#include "CommandLine.h"

using namespace std;

void showUsage(string prog)
{	cerr << endl << "Usage:" << endl
		<< "\t" << prog << " [-h|H][-u|U] -c [-d] [-i] [-e] [-p] [-m] [-f input-file]" << endl;
	return;
}
void showDetailedUsage(string prog)
{ 
	cerr << "The program takes an input delimited-file, extracts fields from 2 identified columns, and outputs" << endl
		<< "to stdout.  If no input-file is entered, the program accepts input from stdin, and requires" << endl
		<< "^Z or EOF to process the input." << endl << endl;
	showUsage(prog); 
	cerr << endl << "where:" << endl
		<< "\t" << "u or U show simple usage" << endl
		<< "\t" << "h or H show this message" << endl 
		<< "\t" << "d flags the column-delimiter used in the input file, uses 'tab' as default" << endl
		<< "\t" << "e flags the (optional) escape character; the '\\' character is taken as default" << endl
		<< "\t" << "c flags the mandatory comma-delimited list of column numbers to \"get\" from the input file" << endl
		<< "\t" << "p flags the path-to-root delimiter to use, default is '<'" << endl
		<< "\t" << "m flags the within-field delimiter to use, default is '|', the input & output w-f delimiter is the same" << endl
		<< "\t" << "i indicates to 'inspect' the first line of the file" << endl
		<< "\t" << "f flags the input filename." << endl << endl;
	return; 
}
vector<string> getFieldData ( const string & line, const string & delimiter );
vector<string> getFieldData (const string & line, const char delimiter, const char escapeChar);
string getPathString (vector<string> & vpath, const string & path_delim ) ;
string getPath ( string child, multimap<string,string> & childPar, string path_delim, string multi_delim);
vector<string> getStatedParents( string & node, const multimap<string,string> & childPar );
unsigned getNumStatedParents( string & node, const multimap<string,string> & childPar );
bool isNodeRoot( string & node, const multimap<string,string> & childPar );  // doesn't have parents
bool isNodeStatedRoot( string & node, const multimap<string,string> & childPar );  // has an "empty" parent, i.e. parent is registered in map
bool isNodeUnstatedRoot( string & node, const multimap<string,string> & childPar ); // i.e. it's not a key in the multimap
void recursePath( string child, const multimap<string,string> & childPar, vector< vector<string> > & mpath, vector<string> & vpath);

int main (int argc, char * argv[])
{
	clock_t tstart, tend;
	tstart = clock();

	// DEAL WITH COMMAND LINE, ARGUMENTS, AND INPUT FILES

	// check command line
	CommandLine cmd(argc, argv);
	if( !cmd.hasAllOptionsIn("cdiepmf") ) {
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
	// string delimiter;
	char delimiter;
	if( cmd.optionFlagsArgument('d') )
		delimiter = (cmd.getFlaggedVariable('d'))[0];
	else {
		delimiter = '\t';	// default value is the tab
		}

	// get escape character for the delimiter, if none stated, use default slash for e.g. \t
	char escape;
	if( cmd.optionFlagsArgument('e') )
		escape = (cmd.getFlaggedVariable('e'))[0];
	else
		escape = '\\';


	// get delimiter for path-to-root nodes, if none stated, use default '<'
	string path_delim;
	if( cmd.optionFlagsArgument('p') )
		path_delim = string( 1, (cmd.getFlaggedVariable('p'))[0] );
	else
		path_delim = "<";

	// get delimiter for multiply-delimited path-to-roots, if none stated, use default '|'
	string multi_delim;
	if( cmd.optionFlagsArgument('m') )
		multi_delim = string( 1, (cmd.getFlaggedVariable('m'))[0] );
	else
		multi_delim = "|";


	// get column numbers to read, exit if 2 columns not stated
	vector<int> col;
	unsigned int col_class = 0, col_super = 0;
	if( !inspect ) {
		if( cmd.optionFlagsArgument('c') ) {
			string strcols = cmd.getFlaggedVariable('c');
			vector<string> colstr = getFieldData(strcols, ",");
			if( colstr.size() == 2 ) {
				col_class = atoi( colstr[0].c_str() );
				col_super = atoi( colstr[1].c_str() );
				}
			else {
				cerr << "Two columns MUST be specified. Exiting program." << endl;
				showUsage(cmd.getProgramName());
				exit(0);
				}
			}
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
	
	// read input, but if user wants only to inspect the data, read one line and output
	string inputLine;
	vector<string> line_list;
	vector< vector<string> > term_list;
	if( inspect ) {
		getline(cin, inputLine);
		cout <<  inputLine << endl;
		return 0;
		}		
	else {
		while( getline(cin, inputLine), !cin.eof())
			if( inputLine.size() > 0 ) {
				line_list.push_back(inputLine);
				term_list.push_back(getFieldData(inputLine, delimiter, escape));
				}
		}
	// unbind cin if necessary and close file
	if( inputfilename != "") {
		cin.rdbuf(backcinstrm);
		inputFile.close();
		}

	// Got the file & got the columns, verify columns are within declared bounds
	if( !( ((col_class-1) >= 0 && (col_class-1) < term_list[0].size()) && ((col_super-1) >= 0 && (col_super-1) < term_list[0].size()) ) ) {
		cerr << "One of the columns exceeds the number of declared fields in a line. Exiting program." << endl;
		showUsage(cmd.getProgramName());
		exit(0);
		}

	// construct the child:parent(s) map, one child can have multiple parents
	multimap<string,string> childPar;
	vector< vector<string> >::iterator pTermList = term_list.begin();
	for( ; pTermList != term_list.end(); ++pTermList ) {
		if( (*pTermList).size() < 2 )
			continue;
		if( (*pTermList)[col_super-1].find(multi_delim) != string::npos ) {
			vector<string> multi_pars = getFieldData ( (*pTermList)[col_super-1], multi_delim );
			vector<string>::iterator pMulti = multi_pars.begin();
			for( ; pMulti != multi_pars.end() ; ++pMulti ) {
				if( *pMulti != "" )
					childPar.insert( pair<string,string>( (*pTermList)[col_class-1], *pMulti) );
				}
			}
		else {
			if( (*pTermList)[col_super-1] != "" )
				childPar.insert( pair<string,string>( (*pTermList)[col_class-1],(*pTermList)[col_super-1] ) );
			}
		}


	// OK, iterate the lines, get the path-to-root and append to each line in output
	string path2root;
	vector<string>::iterator pLineList = line_list.begin();
	pTermList = term_list.begin();
	for( ; pLineList != line_list.end(); ++pLineList, ++pTermList ) {
		path2root = getPath( (*pTermList)[col_class-1], childPar, path_delim, multi_delim);
		cout << *pLineList << delimiter << path2root << endl;
		}
	
	
	

	tend = clock();
	cerr << (tend-tstart)/1000.0 << " seconds elapsed time." << endl;

	return 0;
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
vector<string> getFieldData (const string & line, const char delimiter, const char escapeChar) // accepts escapes & doesn't ignore empty fields
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

string getPath ( string child, multimap<string,string> & childPar, string path_delim, string multi_delim)
{
	string path;
	vector<string> vpath;
	vector< vector<string> > mpath;

	recursePath( child, childPar, mpath, vpath );

	vector< vector<string> >::iterator pMpath = mpath.begin();
	for( ; pMpath != mpath.end(); ++pMpath ) {
		if( pMpath == mpath.begin() ) 
			path += getPathString( *pMpath, path_delim);
		else 
			path += multi_delim + getPathString( *pMpath, path_delim);
		}		

	return path;
}

void recursePath( string child, const multimap<string,string> & childPar, vector< vector<string> > & mpath, vector<string> & vpath)
{
	if( isNodeRoot(child, childPar) ) {
		vpath.push_back(child);
		mpath.push_back(vpath);
		}
	else {	
		unsigned numPars = getNumStatedParents(child, childPar);
		vpath.push_back(child);
		vector<vector<string> > lpath;
		for( unsigned i = 0; i < numPars ; ++i) {
			vector<string> tmp;
			tmp = vpath;
			lpath.push_back(tmp);
			}
		vector<string> pars = getStatedParents( child, childPar );
		vector<string>::iterator pPars = pars.begin();
		vector<vector<string> >::iterator pLpath = lpath.begin();
		for( ; pLpath != lpath.end(), pPars != pars.end() ; ++pLpath, ++pPars) {
			recursePath( *pPars, childPar, mpath, *pLpath );
			}	
		}
	return;
}

string getPathString (vector<string> & vpath, const string & path_delim ) 
{
	string strpath;
	unsigned i = 0;
	vector<string>::iterator pVpath;
	for( pVpath = vpath.begin(); pVpath != vpath.end(); ++pVpath, ++i ) {
		if( i < vpath.size()-1 )
			strpath += *pVpath + path_delim;
		else 
			strpath += *pVpath;
		}
	return strpath;
}
vector<string> getStatedParents( string & node, const multimap<string,string> & childPar )
{
	pair< multimap<string,string>::const_iterator, multimap<string,string>::const_iterator > pRange;
	multimap<string,string>::const_iterator it;
	vector<string> parents;
	pRange = childPar.equal_range(node);
	for( it = pRange.first; it != pRange.second; ++it )
		if( it->second != "" )				// just make sure there are no empty pars in there
			parents.push_back( it->second );
	return parents;
}


unsigned getNumStatedParents( string & node, const multimap<string,string> & childPar )
{
	vector<string> parents = getStatedParents(node, childPar);
	return parents.size();
}


bool isNodeRoot( string & node, const multimap<string,string> & childPar ) // it's either a key in the multimap with "" par, or not a key
{
	if( isNodeUnstatedRoot(node, childPar) || isNodeStatedRoot(node, childPar) )
		return true;
	return false;
}
bool isNodeUnstatedRoot( string & node, const multimap<string,string> & childPar ) // i.e. it's not a key in the multimap
{
	pair< multimap<string,string>::const_iterator, multimap<string,string>::const_iterator > pRange;
	multimap<string,string>::const_iterator it;
	pRange = childPar.equal_range(node);
	if( pRange.first == pRange.second )
		return true;
	return false;
}

bool isNodeStatedRoot( string & node, const multimap<string,string> & childPar )  // i.e. it's a key in the multimap with a "" par
{
	vector<string> pars = getStatedParents(node, childPar);
	vector<string>::iterator it = pars.begin();
	for( ; it != pars.end(); ++it) {
		if( *it == "" )
			return true;
		}
	return false;
}


