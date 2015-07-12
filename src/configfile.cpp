#include <string>
#include <fstream>
#include <vector>

#include "error.hpp"
#include "configfile.hpp"

using namespace std;

/*
Config file format

[SPACE*][QUOTE][SPACE][QUOTE][SPACE][CR*]
* : optional
*/

map<string,string> config_vars;

static bool is_space(char c)
{
	return (c == ' ' || c == '\t');
}

static string read_quote(string s, int &index)
{
	string ret;
	int len=s.size();
	
	if(s[index] == '"') {
		index++; // pass this initial "
		while(s[index] != '"') {
			if(index >= len) {
				error("syntax error");
			}
			else if(s[index] == '\n') {
				error("syntax error");
			}
			ret = ret + s[index];
			index++;
		}
		index++;
	}
	else {
		while(!is_space(s[index])) {
			if(index >= len) {
				// end of input string
				break;
			}
			else if(s[index] == '#') {
				break;
			}
			ret = ret + s[index];
			index++;
		}
	}
	return ret;
}

static string read_space(string s, int &index)
{
	string ret;
	
	while(is_space(s[index])) {
		if(index >= s.size()) {
			// end of input string
			break;
		}
		ret = ret + s[index];
		index++;
	}
	
	return ret;
}

void parse_config_file(string conffile_name)
{
	ifstream conffile;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	conffile.open(conffile_name.c_str());
	if(conffile.fail()) {
		error("unable to open %s", conffile_name.c_str());
	}
	
	while(1) {
		conffile.getline(tmpa, 500); //FIXME: lines limited to 500 chars
		if(!conffile.good()) {
			break;
		}
		
		tmps = tmpa;
		if(tmps[tmps.size()-1] == '\r')
			tmps.erase(tmps.size()-1, 1);
	
		/*
		fields = split_string(tmps, " ");
		if(fields.size() != 2) {
			abort();
		}
		*/
		
		int index=0;
		//printf("\nReading line: %s\n", tmps.c_str());
		read_space(tmps, index);
		string varname = read_quote(tmps, index);
		if(varname.size() == 0) {
			//printf("skipping line\n");
			continue;
		}
		read_space(tmps, index);
		string value = read_quote(tmps, index);
		//printf("got varname [%s] and value [%s]\n", varname.c_str(), value.c_str());
		
		config_vars[varname] = value;
	}
}

void parse_semester_file(string semester_file_name)
{
	ifstream semfile;

	semfile.open(semester_file_name.c_str());
	if(!semfile) {
		error("Unable to open semester file");
	}

	string sem;
	semfile >> sem;

	config_vars["default_semester"] = sem;
}

/*
int main()
{
	parse_config_file("sopti.conf");
	return 0;
}
*/
