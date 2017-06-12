





#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include <functional> 
#include <cctype>
#include <locale>


#include "OntyxKb.h"
#include "OntyxProperty.h"
#include "tString.h"
#include "fullSyn.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2, file3, file4, file5, file6;
	bool	changeSet;
};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void readFlatList(const string &, set<string> & term_list);
bool checkMarkup (const string & inString, const string & startTag, const string & endTag);
string extractFromMarkup (const string &, const string &, const string &);
string getDayStamp (void);
tString trim(tString &);
tString ltrim(tString &);
tString rtrim(tString &);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";
	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\treadNCIOBO obo_file" << endl << endl;
		exit(0);
	}
	else
		ProcessCommandline( commLine, flags);

	tstart = clock();





	ifstream inFile;									
	// VERIFY THAT FILES EXIST
	inFile.open( commLine[1].c_str() );
	if( inFile.bad() ) {
		cerr << "Can't open file '" << commLine[1].c_str() << "'" << endl;
		exit(0);
	}
	bool isTerm = false, hasTerm = false;
	string i_name = "name: ";
	string i_isa = "is_a: ";
	string i_rela = "relationship: ";
	/**
	string i_rela_regu = "relationship: regulates";
	string i_rela_pr = "relationship: positively_regulates";
	string i_rela_nr = "relationship: negatively_regulates";
	string i_rela_ri = "relationship: occurs_in";
	string i_rela_sd = "relationship: starts_during";
	string i_rela_ed = "relationship: ends_during";
	**/
	string i_id = "id: ";
	string i_syn = "synonym: ";
	string i_altid = "alt_id: ";
	string i_def = "def: ";
	string i_comment = "comment: ";
	string i_obsolete = "is_obsolete: ";
	string i_consider = "consider: ";
	string i_replace = "replaced_by: ";
	string i_xref = "xref: ";
	//TODO deal with equivalent classes listed under intersection_of:

	string c_name, code;
	long count = 0;
	vector<string> parents;
	vector<OntyxRole> relations;
	vector<OntyxProperty> synonyms;
	vector<string>::iterator posVec;

	OntyxName tmpName("go", 1, "http://purl.obolibrary.org/obo/go#");
	tdeKb.addNamespace(tmpName);

	OntyxName tmpName2("bfo", 2, "http://purl.obolibrary.org/obo#");
	tdeKb.addNamespace(tmpName2);

	OntyxKind tmpKind("GO_KIND", 1, "GO1", "go", false);
	tdeKb.addKind(tmpKind);

	OntyxKind tmpKind2("BFO_KIND", 2, "GO2", "bfo", false);
	tdeKb.addKind(tmpKind2);


	OntyxRoleDef tmpRole1("part_of", "BFO_0000050", 1, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole1);

	OntyxRoleDef tmpRole2("has_part", "BFO_0000051", 2, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole2);

	OntyxRoleDef tmpRole3("occurs_in", "BFO_0000066", 3, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole3);

	OntyxRoleDef tmpRole4("starts_during", "RO_0002091", 4, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole4);

	OntyxRoleDef tmpRole5("happens_during", "RO_0002092", 5, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole5);

	OntyxRoleDef tmpRole6("ends_during", "RO_0002093", 6, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole6);

	OntyxRoleDef tmpRole7("regulates", "RO_0002211", 7, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole7);

	OntyxRoleDef tmpRole8("negatively_regulates", "RO_0002212", 8, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole8);

	OntyxRoleDef tmpRole9("positively_regulates", "RO_0002213", 9, "bfo", "go", "go", "", "");
	tdeKb.addRoleDef(tmpRole9);


	OntyxPropertyDef tmpProp1("synonym", "synonym", 1, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp1);

	OntyxQualifierDef tmpQual1("synonym_type","synonym_type",2, "go","CP");
	tdeKb.addQualifierDef(tmpQual1);

	OntyxQualifierDef tmpQual2("dbxref","dbxref",3, "go","CP");
	tdeKb.addQualifierDef(tmpQual2);

	OntyxQualifierDef tmpQual3("curator","curator",4, "go","CP");
	tdeKb.addQualifierDef(tmpQual2);


	OntyxPropertyDef tmpProp2("Preferred_Name", "Preferred_Name", 2, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp2);

	OntyxPropertyDef tmpProp3("alt_id", "alt_id", 3, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp3);

	OntyxPropertyDef tmpProp4("def", "definition", 4, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp4);

	OntyxPropertyDef tmpProp5("comment", "comment", 5, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp5);

	OntyxPropertyDef tmpProp6("is_obsolete", "is_obsolete", 6, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp6);

	OntyxPropertyDef tmpProp7("consider", "consider", 7, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp7);

	OntyxPropertyDef tmpProp8("replaced_by", "replaced_by", 8, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp8);

	OntyxPropertyDef tmpProp9("xref", "xref", 9, "string", false, false, "go");
	tdeKb.addPropertyDef(tmpProp9);

	string inputLine;
	while( getline(inFile, inputLine), !inFile.eof()) {
		if( inputLine == "[Term]" ) {
			isTerm = true;
			count++;
			if( count > 1 ) {
				tString t_id = code;

				t_id.tokenize(":");
				string owlCode = t_id.getTokenNumber(0) + "_" + t_id.getTokenNumber(1);

				//Convert GO:code to GO_code for rdf:iD.  Add second "code" property for GO:code

				OntyxConcept tmpConcept(c_name, owlCode, atoi(t_id.getTokenNumber(1).c_str()), "go", "oboInOWL", false);
				tmpConcept.addParents(parents).addRoles(relations).addProperties(synonyms);
				tdeKb.addConcept(tmpConcept);

				relations.clear();
				synonyms.clear();
				parents.clear();
				hasTerm = false;
			}
		}
		else if( inputLine == "[Typedef]" || inFile.eof() ) {
			isTerm = false;
			/**TODO this is all messed up.  It grabs the last concept code and adds it to all
			if( hasTerm ) {
			tString t_id = code;
			t_id.tokenize(":");
			string owlCode = t_id.getTokenNumber(0) + "_" + t_id.getTokenNumber(1);
			OntyxConcept tmpConcept(c_name, owlCode, atoi(t_id.getTokenNumber(1).c_str()), "go", "oboInOWL", false);
			tmpConcept.addParents(parents).addRoles(relations).addProperties(synonyms);
			tdeKb.addConcept(tmpConcept);

			relations.clear();
			synonyms.clear();
			parents.clear();
			hasTerm = false;
			}  **/
		}
		//Properties
		else if( inputLine.substr(0, i_name.size() ) == i_name && isTerm ) {
			string tmp(inputLine, i_name.size());
			OntyxProperty tmpProperty("Preferred_Name", tmp, "");
			synonyms.push_back(tmpProperty);
			hasTerm = true;
		}
		else if( inputLine.substr(0, i_id.size() ) == i_id && isTerm ) {
			string tmp(inputLine, i_id.size());
			code = tmp;
			tString t_id = code;
			t_id.tokenize(":");
			c_name = t_id.getTokenNumber(0) + "_" + t_id.getTokenNumber(1);
		}
		else if( inputLine.substr(0, i_isa.size() ) == i_isa && isTerm ) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(1);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string reference = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			parents.push_back(reference); //.getTokenSequence());
		}
		else if( inputLine.substr(0, i_syn.size() ) == i_syn && isTerm ) {
			//TODO grab the qualifiers and do something with them.
			tString tmpTStr = inputLine;
			tmpTStr.tokenize("\"");
			OntyxProperty tmpProperty("synonym", tmpTStr.getTokenNumber(1), "");
			tString tmpQual_full = tmpTStr.getTokenNumber(2);
			tmpQual_full.tokenize("[");

			tString tmpQual = tmpQual_full.getTokenNumber(0);
			tString tmpQual1 = tmpQual_full.getTokenNumber(1);

			tmpQual.tokenize(" ");
			OntyxQualifier ontQ("synonym_type", tmpQual.getTokenNumber(0));	
			tmpProperty.addQualifier(ontQ);

			std::size_t findBracket = tmpQual1.find_first_of("]");
			if (findBracket!=std::string::npos && findBracket >0) {
				//TODO remove bracket
				tmpQual1 = tmpQual1.replace(findBracket, 1,"");
				tmpQual1.tokenize(",");
			}
			for (int i=0;i<tmpQual1.getNumberOfTokens();i++){
				tString tmpQual_x = tmpQual1.getTokenNumber(i);
				/**				std::size_t found = tmpQual_x.find("GOC");
				if (found!=std::string::npos){
				//todo tokenize on :
				tmpQual_x.tokenize(":");
				OntyxQualifier ontQ_x("curator",tmpQual_x.getTokenNumber(1));
				tmpProperty.addQualifier(ontQ_x);
				}
				else { **/
				OntyxQualifier ontQ_x("dbxref",tmpQual_x);
				tmpProperty.addQualifier(ontQ_x);
				//				}
			}

			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_altid.size()) == i_altid && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(1);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string tmp_name = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			OntyxProperty tmpProperty("alt_id", tmp_name, "");
			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_def.size()) == i_def && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize("\"");
			OntyxProperty tmpProperty("def", tmpTStr.getTokenNumber(1), "");

			tString tmpQual_full = tmpTStr.getTokenNumber(2);
			tmpQual_full.tokenize("[");

			tString tmpQual = tmpQual_full.getTokenNumber(0);
			tString tmpQual1 = tmpQual_full.getTokenNumber(1);

			std::size_t findBracket = tmpQual1.find_first_of("]");
			if (findBracket!=std::string::npos && findBracket >0) {
				//TODO remove bracket
				tmpQual1 = tmpQual1.replace(findBracket, 1,"");
				tmpQual1.tokenize(",");
			}
			for (int i=0;i<tmpQual1.getNumberOfTokens();i++){
				tString tmpQual_x = tmpQual1.getTokenNumber(i);
				tmpQual_x = trim(tmpQual_x);
				OntyxQualifier ontQ_x("dbxref",tmpQual_x);
				tmpProperty.addQualifier(ontQ_x);

			}



			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_comment.size()) == i_comment && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(":");
			tString trimmer = tmpTStr.getTokenNumber(1).substr(1, tmpTStr.getTokenNumber(1).size());
			OntyxProperty tmpProperty("comment", trimmer, "");
			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_obsolete.size()) == i_obsolete && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(": ");
			OntyxProperty tmpProperty("is_obsolete", tmpTStr.getTokenNumber(1), "");
			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_consider.size()) == i_consider && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(1);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string tmp_name = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			OntyxProperty tmpProperty("consider", tmp_name, "");
			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_replace.size()) == i_replace && isTerm) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(1);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string tmp_name = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			OntyxProperty tmpProperty("replaced_by", tmp_name, "");
			synonyms.push_back(tmpProperty);
		}
		else if( inputLine.substr(0, i_xref.size()) == i_xref && isTerm) {
			//Have to handle xref: Reactome:REACT_22383 "Addition of a third mannose to the N-glycan precursor by ALG2, Homo sapiens"
			string tmp(inputLine, i_xref.size());
			OntyxProperty tmpProperty("xref", tmp, "");
			synonyms.push_back(tmpProperty);
		}
		//Relationships
		/**
		else if( inputLine.substr(0, i_rela.size() ) == i_rela && isTerm ) {
		tString tmpTStr = inputLine;
		tmpTStr.tokenize(" ");
		tString tmp2T = tmpTStr.getTokenNumber(2);
		tString p_id = tmp2T;
		p_id.tokenize(":");
		string reference = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
		OntyxRole tmpRole("part_of", reference, "some");
		relations.push_back(tmpRole);
		//			parents.push_back(reference); // .getTokenSequence());	// for the jax owl work, create isas out of roles
		}
		**/
		else if( inputLine.substr(0, i_rela.size() ) == i_rela && isTerm ) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp1T = tmpTStr.getTokenNumber(1);
			tString tmp2T = tmpTStr.getTokenNumber(2);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string reference = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			OntyxRole tmpRole(tmp1T, reference, "some");
			relations.push_back(tmpRole);
			//			parents.push_back(reference); // .getTokenSequence());	// for the jax owl work, create isas out of roles
		}
	}
	//store the last concept and cleanup
	if( count > 1 ) {
				tString t_id = code;
				if(code =="GO:2001316"){
					string bob = "your uncle";
				}

				t_id.tokenize(":");
				string owlCode = t_id.getTokenNumber(0) + "_" + t_id.getTokenNumber(1);

				//Convert GO:code to GO_code for rdf:iD.  Add second "code" property for GO:code

				OntyxConcept tmpConcept(c_name, owlCode, atoi(t_id.getTokenNumber(1).c_str()), "go", "oboInOWL", false);
				tmpConcept.addParents(parents).addRoles(relations).addProperties(synonyms);
				tdeKb.addConcept(tmpConcept);

				relations.clear();
				synonyms.clear();
				parents.clear();
				hasTerm = false;
			}
	inFile.close();


	//TODO change K0, the default kind, to Obsolete with a made-up code


	string filename, daystamp = getDayStamp();


	// ************* WRITE OWL FILE *************
	filename = "GO_OBO-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "header.owl", true);
	//	string makecopy = "copy " + filename + " NCI_Anatomy.owl";
	//	system(makecopy.c_str()); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************

	//Need to do cleanup step here for GO:xxx, or do earlier?



	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


void ProcessCommandline (const vector<string> & commLine, struct comFlags & flags)
{
	for( unsigned i = 1; i < commLine.size(); i++) {
		if( flags.file1 == "" ) {
			flags.file1 = commLine[i];}
		else{
			flags.file2 = commLine[i];}
	}
}

void readFlatList(const string & inFilename, vector<string> & term_list)
{
	ifstream inFile;									// VERIFY THAT FILES EXIST
	inFile.open( inFilename.c_str() );
	if( inFile.bad() ) {
		cerr << "Can't open file '" << inFilename.c_str() << "'" << endl;
		exit(0);
	}

	string inputLine;
	while( getline(inFile, inputLine), !inFile.eof())
		term_list.push_back(inputLine);
	inFile.close();
}

bool checkMarkup (const string & inString, 
				  const string & startTag,
				  const string & endTag)
{
	bool returnFlag = true;
	string newString = "";
	unsigned startPos = inString.find(startTag, 0);
	if( startPos == string::npos) {
		cout << "Bad initial markup \"" << startTag << "\" in '" << inString << "'." << endl;
		returnFlag = false;
		startPos = 0;
	}
	unsigned endPos = inString.find(endTag, startPos);
	if( endPos == string::npos ) {
		cout << "Unterminated markup \"" << endTag << "\" in '" << inString << "'." << endl;
		returnFlag = false;
	}
	return returnFlag;
}


string extractFromMarkup (const string & inString, 
						  const string & startTag,
						  const string & endTag)
{
	string newString = "";
	unsigned startPos = inString.find(startTag, 0);
	if( startPos != string::npos) {
		unsigned endPos = inString.find(endTag, startPos);
		if( endPos != string::npos ){
			newString = inString.substr(startPos + startTag.size(), endPos - startPos - startTag.size());}
		else{
			cout << "Unterminated markup in '" << inString << "'." << endl;}
	}
	return newString;
}

string getDayStamp (void)
{
	char sumFile[10] = {0};
	struct tm when;
	time_t now;   
	char *p;
	char tempS[20] = {0}, monS[3] = {0}, dayS[3] = {0}, yearS[5] = {0};

	time( &now );
	when = *localtime( &now );
	sprintf (yearS, "%d", when.tm_year);	// years since 1900
	if (strlen(yearS) > 2) {		// use only last two digits of year
		p = &yearS[strlen(yearS)-2];
		strcpy(tempS, p);
		strcpy(yearS, tempS);
	} 
	sprintf (monS, "%d", when.tm_mon + 1);
	sprintf (dayS, "%d", when.tm_mday);
	if (strlen(monS) == 1) {		// use month with two digits
		strcpy(tempS, "0");
		strcat(tempS, monS);
		strcpy(monS, tempS);
	}
	if (strlen(dayS) == 1) {		// use days with two digits
		strcpy(tempS, "0");
		strcat(tempS, dayS);
		strcpy(dayS, tempS);
	}
	strcat(sumFile, yearS);
	strcat(sumFile, monS);
	strcat(sumFile, dayS);
	string stamp = sumFile;

	return stamp;
}

string replaceChar(string &s, char c1, char c2)
{
	string t = s;
	for (unsigned int i=0; i<t.size(); i++)
	{
		if (t[i] == c1) t[i] = c2;
	}
	return t;
}

// trim from start
tString ltrim(tString &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
tString rtrim(tString &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
tString trim(tString &s) {
	return ltrim(rtrim(s));
}