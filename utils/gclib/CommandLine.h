#include <vector>
#include <string>
#include <set>
#include <map>

#include <fstream>
#include <iostream>

using namespace std;

class CommandLine
{
	private:
		vector<string>		m_arguments;
		set<char>			m_options;
		vector<string>		m_variables;
		map<char, string>	m_optionVariable;


	public:
		CommandLine (int argc, char *argv[]);
		virtual ~CommandLine ();
		bool hasArguments (void);
		bool hasAllOptionsIn (string);			// pass a string with all accepted/valid options, returns false if a user option is invalid 
		bool hasMandatoryOptions (string);		// pass a string with all mandatory options
		bool hasOption (char);
		bool optionFlagsArgument (char);
		string getVariableByPosition (unsigned int pos);	// elements are ordered as they appear in the command line, 
												// flags are not counted, and [0] is the program name
												// returns empty string if option doesn't exist
		string getFlaggedVariable (char) ;		// returns empty string if option doesn't exist
		int optionCount (void);
		int variableCount (void);				// doesn't count the program name
		string getProgramName (void);


	private:
		void processArguments (void);



}
;


CommandLine::CommandLine (int argc, char *argv[])
{
	for( int i = 0; i < argc; i++ ) {
		string item = argv[i];
		m_arguments.push_back(item);
		}
	processArguments();
}

CommandLine::~CommandLine () {}

bool CommandLine::hasArguments (void)
{
	if( m_arguments.size() > 1 )
		return true;
	return false;
}

void CommandLine::processArguments( void )
{
	char lastOption = 0;
	vector<string>::iterator pArg = m_arguments.begin();
	for( ; pArg != m_arguments.end(); ++pArg ) {
		if( (*pArg)[0] == '-' ) {
			if( pArg->size() > 1 ) {					// it's a flag or collection of flags
				unsigned int i;
				for( i = 1; i < pArg->size(); i++) {
					m_options.insert((*pArg)[i]);
					}
				lastOption = (*pArg)[pArg->size()-1];
				}
			}
		else {										// it's an input variable, [0] is the program name
			m_variables.push_back(*pArg);
			if( lastOption != 0 ) {
				m_optionVariable[lastOption] = *pArg;
				lastOption = 0;
				}
			}
		}
}	

bool CommandLine::hasAllOptionsIn (string validFlags)
{
	set<char>::iterator pOps = m_options.begin();
	for( ; pOps != m_options.end(); ++pOps ) {
		if( validFlags.find(*pOps) == string::npos )
			return false;
		}
	return true;
}	

bool CommandLine::hasMandatoryOptions (string mandFlags)
{	// doesn't check for empty string
	string::iterator pOps = mandFlags.begin();
	for( ; pOps != mandFlags.end(); ++pOps ) {
		if( m_options.find(*pOps) == m_options.end() )
			return false;
		}
	return true;
}	

bool CommandLine::hasOption( char c )
{
	if( m_options.find(c) != m_options.end() )
		return true;
	return false;
}

int CommandLine::optionCount (void)
{
	return m_options.size();
}

int CommandLine::variableCount (void)
{
	return m_variables.size() - 1;
}

string CommandLine::getVariableByPosition (unsigned int pos) 
{
	if( pos < m_variables.size() )
		return m_variables[pos];
	return "";
}

string CommandLine::getFlaggedVariable (char c)
{
	if( optionFlagsArgument(c) )
		return m_optionVariable[c];
	return "";
}

bool CommandLine::optionFlagsArgument (char c)
{
	if( m_optionVariable.find(c) != m_optionVariable.end() )
		return true;
	return false;
}

string CommandLine::getProgramName (void)
{
	size_t pos;
	if( (pos = m_variables[0].find_last_of("/\\")) != string::npos )
		return m_variables[0].substr(pos+1);
	return m_variables[0];
}




// driver with examples
// uncomment below and rename to .cpp to compile and test

/*
void showUsage(string prog)
{
	cout << endl << "Usage:" << endl
		<< "\t" << prog << " -[a][b][c][D][e][H] -F input-file [-O output-file]" << endl;
	return;
}
	
int main(int argc, char* argv[])
{
	CommandLine cmd(argc, argv);
	cout << "has arguments = " << cmd.hasArguments() << endl;
	cout << "number of variables = " << cmd.variableCount() << endl;
	cout << "number of flags = " << cmd.optionCount() << endl;
	cout << "has option \"c\" = " << cmd.hasOption('c') << endl;
	cout << "are all entered options valid (valid = a->f) = " << cmd.hasAllOptionsIn("abcdef") << endl;
	cout << "does option \"c\" flag variable = " << cmd.optionFlagsArgument('c') << endl;
	cout << "get variable flagged by \"c\" = " << cmd.getFlaggedVariable('c') << endl;
	cout << "get variable by position 1 = " << cmd.getVariableByPosition(1) << endl;
	cout << "get program name = " << cmd.getProgramName() << endl;

	if( !cmd.hasAllOptionsIn("abcDeFO") || cmd.hasOption('H') || !cmd.hasArguments() )
		showUsage(cmd.getProgramName());

	return 0;
}
*/


