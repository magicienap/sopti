#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

#include <getopt.h>

#include "globals.hpp"
#include "schoolschedule.hpp"
#include "schoolcourse.hpp"

using namespace std;

#define COURSEFILE_FIELD_CYCLE		0
#define COURSEFILE_FIELD_SYMBOL		1
#define COURSEFILE_FIELD_GROUP		2
#define COURSEFILE_FIELD_CREDITS	3
#define COURSEFILE_FIELD_ROOM		6
#define COURSEFILE_FIELD_COURSELAB	7
#define COURSEFILE_FIELD_TITLE		11


struct Action {
	char name[20];
	void (*func)(int, char **);
};

void listcourses(int, char **);
void make(int, char **);

struct Action actions[] =
	{ { "listcourses", listcourses},
	  { "make", make},
	  { 0, 0 } };

string config_file="sopti.conf";
string action;
SchoolSchedule schoolsched;

class SchoolCoursePtrSymAlphaOrder
{
	public:
	SchoolCoursePtrSymAlphaOrder() {}
	bool operator()(const SchoolCourse *a, const SchoolCourse *b) {
		return a->symbol() < b->symbol();
	}
};


void listcourses(int, char **)
{
	vector<SchoolCourse *>::iterator it;
	vector<SchoolCourse *> l_courses;
	SchoolCourse::group_list_t::const_iterator it2;
	
	// Sort the courses before printing them
	l_courses.insert(l_courses.end(), schoolsched.courses_begin(), schoolsched.courses_end());
	sort<vector<SchoolCourse *>::iterator, SchoolCoursePtrSymAlphaOrder>(l_courses.begin(), l_courses.end(), SchoolCoursePtrSymAlphaOrder());

	for(it=l_courses.begin(); it!=l_courses.end(); it++) {
		cout << (*it)->symbol() << " " << (*it)->title();
		for(it2=(*it)->groups_begin(); it2!=(*it)->groups_end(); it2++) {
			cout << " " << (*it2)->name() << "(" << ((*it2)->lab()?"L":"T") << ")";
		}
		cout << endl;
	}
}

void make(int argc, char **argv)
{
	vector<string> requested_courses;
	int max_scheds=10;
	
	int c;
	
	opterr = 0;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"course", 1, 0, 'c'},
			{"count", 1, 0, 'n'},
			{"objective", 1, 0, 'J'},
			{"objective-args", 1, 0, 'j'},
			{"constraint", 1, 0, 'T'},
			{"constraint-args", 1, 0, 't'},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (argc, argv, "cnJjTt",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
			case 'c':
				requested_courses.push_back(optarg);
				break;
				
			case 'n':
				max_scheds=atoi(optarg);
				break;
				
			case 'J':
				break;
				
			case 'j':
				break;
				
			case 'T':
				break;
				
			case 't':
				break;
	
			case '?':
				error("invalid argument");
				break;
		
			default:
				error("getopt returned unknown code %d\n", c);
		}
	}
}

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

void load_info_from_csv(SchoolSchedule *sopti, string periods_file, string closed_file)
{
	ifstream period_list;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	period_list.open(periods_file.c_str());
	// Check error on open
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
		
		// extract vars
		bool islab;
				
		if(fields[COURSEFILE_FIELD_COURSELAB] == "L")
			islab=1;
		else if(fields[COURSEFILE_FIELD_COURSELAB] == "C")
			islab=0;
		else
			error("unrecognized course type");
		
		
		// Add course if not exists
		if(!sopti->course_exists(fields[COURSEFILE_FIELD_SYMBOL])) {
			SchoolCourse newcourse(fields[COURSEFILE_FIELD_SYMBOL]);
			newcourse.set_title(fields[COURSEFILE_FIELD_TITLE]);
			sopti->course_add(newcourse);
		}
		
		// Add group if not exists
		if(!sopti->course(fields[COURSEFILE_FIELD_SYMBOL])->group_exists(fields[COURSEFILE_FIELD_GROUP], islab)) {
			Group newgroup(fields[COURSEFILE_FIELD_GROUP]);
			newgroup.set_lab(islab);

			sopti->course(fields[COURSEFILE_FIELD_SYMBOL])->add_group(newgroup, islab);
		}
		
		Period newperiod;
		newperiod.set_room(fields[COURSEFILE_FIELD_ROOM]);
		sopti->course(fields[COURSEFILE_FIELD_SYMBOL])->group(fields[COURSEFILE_FIELD_GROUP], islab)->add_period(newperiod);
	}
}

void set_default_options()
{
	config_file = "sopti.conf";
}

void parse_command_line(int *argc, char ***argv)
{
	int c;
	
	opterr = 0;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (*argc, *argv, "+h",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
	
		case '?':
			error("invalid argument");
			break;
	
		default:
			error("getopt returned unknown code %d\n", c);
		}
	}

	if(optind >= *argc) {
		error("no action specified on command line");
	}
	
	*argc -= optind;
	*argv += optind;
	
	action = **argv;
	
	*argc--;
	*argv--;
}

void parse_config_file(string conffile_name)
{
	ifstream conffile;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	conffile.open(conffile_name.c_str());
	
	while(1) {
		conffile.getline(tmpa, 500);
		if(!conffile.good()) {
			break;
		}
		
		tmps = tmpa;
		if(tmps[tmps.size()-1] == '\r')
			tmps.erase(tmps.size()-1, 1);
	
		fields = split_string(tmps, " ");
		if(fields.size() != 2) {
			abort();
		}
		
		//options[fields[0]] = fields[1];
	}
}

int main(int argc, char **argv)
{
	int i;
	
	set_default_options();
	parse_command_line(&argc, &argv);
	parse_config_file(config_file);
	
	load_info_from_csv(&schoolsched, "data/courses.csv", "Fermes.csv");
	
	debug("selected action: %s", action.c_str());
	
	for(i=0; actions[i].name[0] != 0; i++) {
		if(action == actions[i].name) {
			if(actions[i].func) {
				actions[i].func(argc, argv);
			}
			goto found;
		}
	}
	
	error("action not found: %s", action.c_str());
	
	found:
	
	return 0;
}
