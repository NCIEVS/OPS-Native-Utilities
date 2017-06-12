#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "utf8.h"

using namespace std;


int main(int argc, char** argv)
{
	if (argc != 2) {
		cout << "\nUsage: checkutf8 filename\n";
		return 0;
		}

	const char* test_file_path = argv[1];
	// Open the test file (contains UTF-8 encoded text)
	ifstream fs8(test_file_path);
	if (!fs8.is_open()) {
	cout << "Could not open " << test_file_path << endl;
	return 0;
		}

	unsigned line_count = 1;
	string line;
	// Play with all the lines in the file
	while (getline(fs8, line)) {
	   // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
		string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
		if (end_it != line.end()) {
			cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
			cout << "This part is fine: " << string(line.begin(), end_it) << "\n";
			}
		line_count++;
		}

	return 0;
}

/*
bool valid_utf8_file(iconst char* file_name)
{
	ifstream ifs(file_name);
	if (!ifs)
		return false; // even better, throw here

	istreambuf_iterator<char> it(ifs.rdbuf());
	istreambuf_iterator<char> eos;

	return utf8::is_valid(it, eos);
}
*/
