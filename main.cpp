/*  School Schedule Optimizer
 *  Copyright (C) 2004  Pierre-Marc Fournier <pmf@users.sf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

#include <getopt.h>
#include <string.h>

#include "globals.hpp"
#include "schoolschedule.hpp"
#include "schoolcourse.hpp"
#include "studentschedule.hpp"
#include "constraint.hpp"

using namespace std;

#define COURSEFILE_FIELD_CYCLE		0
#define COURSEFILE_FIELD_SYMBOL		1
#define COURSEFILE_FIELD_GROUP		2
#define COURSEFILE_FIELD_CREDITS	3
#define COURSEFILE_FIELD_ROOM		6
#define COURSEFILE_FIELD_COURSELAB	7
#define COURSEFILE_FIELD_LABWEEK	9
#define COURSEFILE_FIELD_COURSETYPE	10
#define COURSEFILE_FIELD_TITLE		11
#define COURSEFILE_FIELD_DAY		13
#define COURSEFILE_FIELD_TIME		14


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
string course_file="data/courses.csv";
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

void print_schedule(StudentSchedule &s)
{
	char days_of_week[][9] = { "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche" };
	int hours_week[] = { 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 };
	int hours_weekend[] = { 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, -1 };
	int i,j,k;
	int num_lines=2; // Number of lines allocated for each period
			 // Line 1: Course number and T or L for theorical or lab
			 // Line 2: Room number and group
	
	vector<map<int, vector<string> > > sched;
	sched.resize(7);
	
	StudentSchedule::course_list_t::const_iterator it;
	Group::period_list_t::const_iterator it2;
	for(it=s.st_courses_begin(); it!=s.st_courses_end(); it++) {
		// If has theorical class
		if((*it)->theory_group) {
			for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
				int num_day = (*it2)->period_no()/10000-1;
				
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)].push_back((*it)->course->symbol() + "(T)");
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)].push_back((*it2)->room() + "(" + (*it)->theory_group->name() + ")");
			}
		}
		// If has lab class
		if((*it)->lab_group) {
			for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
				int num_day = (*it2)->period_no()/10000-1;
				string lab_week_str;
				
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)].push_back((*it)->course->symbol() + "(L)");
				
				if((*it2)->week()) {
					char *tmp;
					asprintf(&tmp, "%d", (*it2)->week());
					lab_week_str = string("B") + string(tmp);
					free(tmp);
				}
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)].push_back((*it2)->room() + "(" + (*it)->lab_group->name() + ") " + lab_week_str);
			}
		}
	}
	
	for(i=0; i<7; i++) {
		if(i == 5)
			printf(" | ");
	
		printf("%-15s", days_of_week[i]);
	}
	printf("\n");

	bool week_finished=false;
	bool weekend_finished=false;
	
	// For each hour
	for(i=0;; i++) {
	
		if(!week_finished && hours_week[i] == -1)
			week_finished = true;
			
		if(!weekend_finished && hours_weekend[i] == -1)
			weekend_finished = true;
		
		if(week_finished && weekend_finished)
			break;
	
		// Print hour
		for(j=0; j<7; j++) {
			if(j == 5)
				printf(" | ");
		
			if(j < 5 && !week_finished) {
				printf("%-15d", hours_week[i]);
			}
			else if(j >=5 && !weekend_finished){
				printf("%-15d", hours_weekend[i]);
			}
		}
		
		printf("\n");
		
		for(k=0; k<num_lines; k++) {
			for(j=0; j<7; j++) {
				if(j == 5)
					printf(" | ");
			
				if(j < 5 && !week_finished) {
					if(sched[j].find(hours_week[i]) != sched[i].end() && sched[j][hours_week[i]].size() > k) {
						printf("%-15s", sched[j][hours_week[i]][k].c_str());
					}
					else {
						printf("               ");
					}
				}
				else if(j >=5 && !weekend_finished){
					if(sched[j].find(hours_weekend[i]) != sched[i].end() && sched[j][hours_weekend[i]].size() > k) {
						printf("%-15s", sched[j][hours_weekend[i]][k].c_str());
					}
					else {
						printf("               ");
					}
				}
			}
			printf("\n");
		}
		
		printf("\n");
	}

	
	printf("\n");
}

void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> &constraints, vector<StudentSchedule> &solutions);

void make(int argc, char **argv)
{
	vector<string> requested_courses;
	vector<Constraint *> constraints;
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
		c = getopt_long (argc, argv, "c:n:J:j:T:t:",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
			case 'c':
				if(!schoolsched.course_exists(optarg)) {
					error("course does not exist: %s", optarg);
				}
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
				if(!strcmp(optarg, "noevening")) {
					constraints.push_back(new NoEvening());
				}
				else {
					error("unknown constaint (%s)", optarg);
				}
				break;
				
			case 't':
				break;
	
			case '?':
				error("invalid command line");
				break;
		
			default:
				error("getopt returned unknown code %d\n", c);
		}
	}
	
	// Making an empty schedule, to begin the recursion with
	StudentSchedule sched;
	vector<StudentSchedule> solutions;
	
	// Prepare constraints
	NoConflicts noc;
	constraints.push_back(&noc);
	
	make_recurse(sched, requested_courses, constraints, solutions);
	
	vector<StudentSchedule>::iterator it;
	for(it=solutions.begin(); it!=solutions.end(); it++) {
		print_schedule(*it);
	}
	
	debug("got %d solutions!", solutions.size());
}

void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> &constraints, vector<StudentSchedule> &solutions);

/* Test the student schedule with the constraints before continuing the recursion */

void test_and_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> &constraints, vector<StudentSchedule> &solutions)
{
	vector<Constraint *>::iterator it;
	for(it=constraints.begin(); it!=constraints.end(); it++) {
		if(!((**it)(ss))) {
			debug("constraint failed");
			return;
		}
	}
	
	// If we get here, all the constraints were satisfied
	
	make_recurse(ss, remaining_courses, constraints, solutions);
}

void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> &constraints, vector<StudentSchedule> &solutions)
{
	string course_to_add;

	if(remaining_courses.size() == 0) {
		solutions.push_back(ss);
		return;
	}
	
	course_to_add = remaining_courses.back();
	remaining_courses.pop_back();

	StudentCourse newcourse;
	newcourse.course = schoolsched.course(course_to_add);
		
	SchoolCourse::group_list_t::const_iterator it,it2;
	
	if(schoolsched.course(course_to_add)->type() == COURSE_TYPE_THEORYONLY){
		// This course has only theorical sessions
		for(it=schoolsched.course(course_to_add)->groups_begin(); it!=schoolsched.course(course_to_add)->groups_end(); it++) {
			if(!(*it)->lab()) {
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				newcourse.lab_group = 0;
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, solutions);
			}
		}
	}
	else if(schoolsched.course(course_to_add)->type() == COURSE_TYPE_LABONLY){
		for(it=schoolsched.course(course_to_add)->groups_begin(); it!=schoolsched.course(course_to_add)->groups_end(); it++) {
			if((*it)->lab()) {
				StudentSchedule tmps(ss);
				newcourse.theory_group = 0;
				newcourse.lab_group = (*it);
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, solutions);
			}
		}
	}
	else if(schoolsched.course(course_to_add)->type() == COURSE_TYPE_THEORYLABIND){
		for(it=schoolsched.course(course_to_add)->groups_begin(); it!=schoolsched.course(course_to_add)->groups_end(); it++) {
			if(!(*it)->lab()) {
				newcourse.theory_group = (*it);
				
				// Try all labs
				
				for(it2=schoolsched.course(course_to_add)->groups_begin(); it2!=schoolsched.course(course_to_add)->groups_end(); it2++) {
					if((*it2)->lab()) {
						StudentSchedule tmps(ss);
						newcourse.lab_group = (*it2);
						tmps.add_st_course(newcourse);
						test_and_recurse(tmps, remaining_courses, constraints, solutions);
					}
				}
			}
		}
	}
	else if(schoolsched.course(course_to_add)->type() == COURSE_TYPE_THEORYLABSAME){
		for(it=schoolsched.course(course_to_add)->groups_begin(); it!=schoolsched.course(course_to_add)->groups_end(); it++) {
			if(!(*it)->lab()) {
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				if(!schoolsched.course(course_to_add)->group_exists((*it)->name(), true)) {
					error("a lab group does not exist for a course with same-lab-group policy");
				}
				
				newcourse.lab_group = schoolsched.course(course_to_add)->group((*it)->name(), true);
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, solutions);
			}
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

int poly_period_to_time(int period)
{
	return period%10000;
}

int poly_make_period_no(string day_of_week, string time_str)
{
	int retval;
	char *err;
	
	// Convert the time to integer
	retval = strtol(time_str.c_str(), &err, 10);
	if(*err != 0) {
		error("poly_make_period_no: non-numerical time");
	}
	
	// Check if the time makes sense
	if(retval > 2400 || retval < 0) {
		error("poly_make_period_no: time out of range");
	}
	
	if(day_of_week == "LUN") {
		retval += 10000;
	}
	else if(day_of_week == "MAR") {
		retval += 20000;
	}
	else if(day_of_week == "MER") {
		retval += 30000;
	}
	else if(day_of_week == "JEU") {
		retval += 40000;
	}
	else if(day_of_week == "VEN") {
		retval += 50000;
	}
	else if(day_of_week == "SAM") {
		retval += 60000;
	}
	else if(day_of_week == "DIM") {
		retval += 70000;
	}
	else {
		error("poly_make_period_no: invalid day of week");
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
		int type;
				
		if(fields[COURSEFILE_FIELD_COURSELAB] == "L")
			islab=1;
		else if(fields[COURSEFILE_FIELD_COURSELAB] == "C")
			islab=0;
		else
			error("unrecognized course type");
		
		if(fields[COURSEFILE_FIELD_COURSETYPE] == "T") {
			type = COURSE_TYPE_THEORYONLY;
		}
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "L") {
			type = COURSE_TYPE_LABONLY;
		}
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "TL") {
			type = COURSE_TYPE_THEORYLABSAME;
		}
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "TLS") {
			type = COURSE_TYPE_THEORYLABIND;
		}
		
		// Add course if not exists
		if(!sopti->course_exists(fields[COURSEFILE_FIELD_SYMBOL])) {
			SchoolCourse newcourse(fields[COURSEFILE_FIELD_SYMBOL]);
			newcourse.set_title(fields[COURSEFILE_FIELD_TITLE]);
			newcourse.set_type(type);
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
		newperiod.set_period_no(poly_make_period_no(fields[COURSEFILE_FIELD_DAY], fields[COURSEFILE_FIELD_TIME]));
		if(fields[COURSEFILE_FIELD_LABWEEK] == "I") {
			newperiod.set_week(1);
		}
		else if(fields[COURSEFILE_FIELD_LABWEEK] == "P") {
			newperiod.set_week(2);
		}
		else {
			newperiod.set_week(0);
		}
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
			{"coursefile", 1, 0, 2},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (*argc, *argv, "+h",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
	
		case 2:
			course_file=optarg;
			break;
		
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
	
	load_info_from_csv(&schoolsched, course_file, "Fermes.csv");
	
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
