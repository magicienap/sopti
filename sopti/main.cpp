#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

#include "sopti.hpp"
#include "course.hpp"

using namespace std;

#define COURSEFILE_FIELD_CYCLE		0
#define COURSEFILE_FIELD_SYMBOL		1
#define COURSEFILE_FIELD_GROUP		2
#define COURSEFILE_FIELD_CREDITS	3
#define COURSEFILE_FIELD_ROOM		6
#define COURSEFILE_FIELD_COURSELAB	7
#define COURSEFILE_FIELD_TITLE		11

vector<string> split_string(string s, string sep)
{
	unsigned int pos=0;
	unsigned int start=0; // The starting position of the next search
	int len; // The length of the next substring
	vector<string> retval;

	while(start < s.length()) {
		pos = s.find(sep, start);
		if(pos == string::npos) {
			len = s.length() - start;
		}
		else {
			len = pos - start;
		}
		retval.push_back(s.substr(start, len));
		start = start + len + sep.length();
	}

	return retval;
}

void load_info_from_csv(Sopti *sopti, string periods_file, string closed_file)
{
	ifstream period_list;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	period_list.open(periods_file.c_str());
	// Ignore header line
	period_list.getline(tmpa, 500);
	
	while(1) {
		period_list.getline(tmpa, 500);
		if(!period_list.good()) {
			break;
		}
		
		tmps = tmpa;
		if(tmps[tmps.size()-1] == '\r')
			tmps.erase(tmps.size()-1, 1);
	
		fields = split_string(tmps, ";");
		if(fields.size() != 15) {
			abort();
		}
		
		// Add course if not exists
		if(!sopti->course_exists(fields[COURSEFILE_FIELD_SYMBOL])) {
			Course newcourse(fields[COURSEFILE_FIELD_SYMBOL]);
			newcourse.set_title(fields[COURSEFILE_FIELD_TITLE]);
			sopti->course_add(newcourse);
		}
		
		// Add group if not exists
		Group newgroup(atoi(fields[COURSEFILE_FIELD_GROUP]));
		newgroup.set_
		
		
		/*
		Period newperiod;
		sopti->courses(fields[COURSEFILE_FIELD_SYMBOL]).add_period(newperiod);
		*/
	}
}

void set_default_options(vector<string> options)
{
	options["config_file"] = "sopti.conf";
}

void parse_command_line(int argc, char **argv, vector<string> options)
{
}

void parse_config_file(string conffile_name, map<string> &options)
{
	ifstream conffile;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	conffile.open(conffile_name.c_str());
	
	while(1) {
		conffile.getline(tmpa, 500);
		if(!period_list.good()) {
			break;
		}
		
		tmps = tmpa;
		if(tmps[tmps.size()-1] == '\r')
			tmps.erase(tmps.size()-1, 1);
	
		fields = split_string(tmps, " ");
		if(fields.size() != 2) {
			abort();
		}
		
		options[fields[0]] = fields[1];
	}
}

int main(int argc, char **argv)
{
	Sopti s;
	
	map<string> options;
	
	set_default_options(options);
	parse_command_line(options);
	parse_config_file(options["config_file"], options);
	
	load_info_from_csv(&s, "Horsage.csv", "Fermes.csv");
	
	Sopti::course_list_t::const_iterator it;
	for(it = s.courses_begin(); it!=s.courses_end(); it++) {
		cout << it->symbol() << " " << it->title() << endl;
	}
}
