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
#include "objective.hpp"

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

#define OUTPUT_HTML	1

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
int output_fmt=0;

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

void print_schedule_ascii(StudentSchedule &s)
{
	char days_of_week[][9] = { "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche" };
	int hours_week[] = { 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 };
	int hours_weekend[] = { 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, -1 };
	unsigned int i,j,k;
	unsigned int num_lines=2; // Number of lines allocated for each period
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

void print_schedule_html(StudentSchedule &s)
{
	char days_of_week[][9] = { "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche" };
	int hours_week[] = { 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 };
	int hours_weekend[] = { 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, -1 };
	unsigned int i,j,k;
	unsigned int num_lines=2; // Number of lines allocated for each period
			 // Line 1: Course number and T or L for theorical or lab
			 // Line 2: Room number and group
	
	vector<map<int, string > > sched;
	sched.resize(7);
	
	StudentSchedule::course_list_t::const_iterator it;
	Group::period_list_t::const_iterator it2;
	for(it=s.st_courses_begin(); it!=s.st_courses_end(); it++) {
		// If has theorical class
		if((*it)->theory_group) {
			for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
				int num_day = (*it2)->period_no()/10000-1;
				
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)] += ((*it)->course->symbol() + "(T)" + "<br>");
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)] += ((*it2)->room() + "(" + (*it)->theory_group->name() + ")");
			}
		}
		// If has lab class
		if((*it)->lab_group) {
			for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
				int num_day = (*it2)->period_no()/10000-1;
				string lab_week_str;
				
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)] += ((*it)->course->symbol() + "(L)" + "<br>");
				
				if((*it2)->week()) {
					char *tmp;
					asprintf(&tmp, "%d", (*it2)->week());
					lab_week_str = string("B") + string(tmp);
					free(tmp);
				}
				sched[num_day][(*it2)->period_no()%((num_day+1)*10000)] += ((*it2)->room() + "(" + (*it)->lab_group->name() + ") " + lab_week_str);
			}
		}
	}
	
	printf("<table class=\"schedule\">\n");
	
	printf("<tr>\n");
	
	for(i=0; i<7; i++) {
		printf("<td class=\"weekday\">%s</td>", days_of_week[i]);
	}
	
	printf("</tr>\n");

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
		
		
		printf("<tr>\n");
		for(j=0; j<7; j++) {
			printf("<td class=\"period\">");
			if(j < 5 && !week_finished) {
				printf("<b>%d</b><br>", hours_week[i]);
				if(sched[j].find(hours_week[i]) != sched[i].end()) {
					printf("%s", sched[j][hours_week[i]].c_str());
				}
			}
			else if(j >=5 && !weekend_finished){
				printf("<b>%d</b><br>", hours_weekend[i]);
				if(sched[j].find(hours_weekend[i]) != sched[i].end()) {
					printf("%s", sched[j][hours_weekend[i]].c_str());
				}
			}
			printf("</td>");
		}
		printf("</tr>\n");
		
	}

	printf("</table>\n");
}


/*
void print_schedule_html2(StudentSchedule &s)
{
	char days_of_week[][9] = { "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche" };
	int hours_week[] = { 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 };
	int hours_weekend[] = { 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, -1 };
	int hours_whole_week = {
		{ 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 }, // Monday
		{ 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 }, // Tuesday
		{ 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 }, // Wednesday
		{ 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 }, // Thursday
		{ 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, 1800, 1900, 2000, 2100, -1 }, // Friday
		{ 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600,   -1,   -1,   -1,   -1, -1 }, // Saturday
		{ 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600,   -1,   -1,   -1,   -1, -1 }  // Sunday
	};
	
	unsigned int i,j,k;
	unsigned int num_lines=2; // Number of lines allocated for each period
			 // Line 1: Course number and T or L for theorical or lab
			 // Line 2: Room number and group
	
	vector<map<int, vector<string> > > sched;
	sched.resize(7);
	
	// Load the data in the structure
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
	
		// Print hour for each day
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
*/

void print_schedule(StudentSchedule &s)
{
	if(output_fmt == OUTPUT_HTML)
		print_schedule_html(s);
	else
		print_schedule_ascii(s);
}

void usage()
{
	const char usage_text[]=
	"Usage:\n"
	"sopti [GLOBAL_OPTIONS] ACTION [ACTION_OPTIONS]\n"
	"\n"
	"Actions:\n"
	"  listcourses - print a list of courses\n"
	"  make - make schedules\n"
	"\n"
	"Global options:\n"
	"  --course_file <course_file> - specify course file explicitly\n"
	"\n"
	"Action options:\n"
	"  * make\n"
	"    -c <course> - add this course to the schedule\n"
	"                  (repeat this option for each course)\n"
	"    -J <objective> - order results by criteria (minholes)\n"
	"    -T <constraint> - use constraint (noevening)\n"
	"\n";
	
	fprintf(stderr, usage_text);
}

void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> &constraints, vector<StudentSchedule> &solutions);

void make(int argc, char **argv)
{
	vector<string> requested_courses;
	vector<Constraint *> constraints;
	int max_scheds=10;
	Objective *objective=0;
	
	int c;
	
	opterr = 0;
	optind=1;

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
				if(!strcmp(optarg, "minholes")) {
					objective = new MinHoles();
				}
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
	
	if(objective) {
		multimap<float, StudentSchedule *> scores;
		vector<StudentSchedule>::iterator it;
		multimap<float, StudentSchedule *>::const_iterator it2;
		
		// Order the solutions by score
		for(it=solutions.begin(); it!=solutions.end(); it++) {
			// Caution, if 2 have the same score, it will not insert
			scores.insert(pair<float, StudentSchedule *>(objective->operator()(&*it), &*it));
		}
		// Print the solutions in order
		for(it2=scores.begin(); it2!=scores.end(); it2++) {
			printf("Score: %f\n",it2->first);
			print_schedule(*(it2->second));
			
		}

	}
	else {
		vector<StudentSchedule>::iterator it;
		for(it=solutions.begin(); it!=solutions.end(); it++) {
			print_schedule(*it);
		}
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
	
	// Open the file
	period_list.open(periods_file.c_str());
	if(period_list.fail()) {
		error("opening file %s", periods_file.c_str());
	}
	
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
			error("invalid course file format (field_num != 15)");
		}
		
		// prepare some variables in advance
		bool islab;
		int type;
				
		if(fields[COURSEFILE_FIELD_COURSELAB] == "L")
			islab=true;
		else if(fields[COURSEFILE_FIELD_COURSELAB] == "C")
			islab=false;
		else
			error("unrecognized course type");
		
		if(fields[COURSEFILE_FIELD_COURSETYPE] == "T")
			type = COURSE_TYPE_THEORYONLY;
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "L")
			type = COURSE_TYPE_LABONLY;
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "TL")
			type = COURSE_TYPE_THEORYLABSAME;
		else if(fields[COURSEFILE_FIELD_COURSETYPE] == "TLS")
			type = COURSE_TYPE_THEORYLABIND;
		
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
		
		// Add period. It should not exist.
		
		int period_no = poly_make_period_no(fields[COURSEFILE_FIELD_DAY], fields[COURSEFILE_FIELD_TIME]);
		if(sopti->course(fields[COURSEFILE_FIELD_SYMBOL])->group(fields[COURSEFILE_FIELD_GROUP], islab)->has_period(period_no)) {
			error("two occurences of the same period in course file");
		}
		
		Period newperiod;
		newperiod.set_room(fields[COURSEFILE_FIELD_ROOM]);
		newperiod.set_period_no(period_no);
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
	
	period_list.close();
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
			{"coursefile", required_argument, 0, 'c'},
			{"html", 0, 0, 3},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (*argc, *argv, "+hc:", long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
	
		case 'c':
			course_file=optarg;
			break;
			
		case 3:
			output_fmt = OUTPUT_HTML;
			break;
		
		case '?':
			usage();
			error("invalid argument");
			break;
	
		default:
			error("getopt returned unknown code %d\n", c);
		}
	}

	if(optind >= *argc) {
		usage();
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
	
	for(i=0; actions[i].name[0] != 0; i++) {
		if(action == actions[i].name) {
			if(actions[i].func) {
				actions[i].func(argc, argv);
			}
			goto found;
		}
	}
	
	// Action not found
	usage();
	error("action not found: %s", action.c_str());
	
	found:
	
	// Action was found and executed
	return 0;
}
