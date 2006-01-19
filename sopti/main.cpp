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
 
#include "main.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "globals.hpp"
#include "make_message.h"
#include "schoolschedule.hpp"
#include "group_constraint.hpp"
#include "objective.hpp"
#include "stdsched.h"
#include "read_csv.hpp"
#include "dbloader.hpp"
#include "configfile.hpp"
#include "uepoch.h"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#else
#error "Don't have config.h"
#endif

struct Action actions[] = {
		{ "listcourses", listcourses},
		{ "make", make},
		{ 0, 0 }
	};


/* --------------------------------------------------------------------------

	Function: listcourses
	Description: Outputs a list of the courses and groups available
		in the database
	Parameters: none
	Return value: none
	Notes: implements the model of an "action"

-------------------------------------------------------------------------- */

void listcourses(int, char **)
{
	vector<string>::const_iterator it;
	vector<SchoolCourse *> l_courses;
	SchoolCourse::group_list_t::const_iterator it2;
	
	DBLoader dbl;
	vector<string> courses;
	courses = dbl.get_course_list();
	
	// Sort the courses before printing them
	
	for(it=courses.begin(); it!=courses.end(); it++) {
		cout << *it << endl;
	}
}


/* ------------------------------------------------------------------

	Function: print_schedule_ascii
	Description: Outputs a schedule in plain text format
	Parameters: s, the schedule to output
	Return value: none
	Notes: none

------------------------------------------------------------------ */

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
					tmp = make_message("%d", (*it2)->week());
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

void print_schedule_xml_groupdefs(StudentSchedule &s, float score)
{
	StudentSchedule::course_list_t::const_iterator it;
	string th_grp;
	string lab_grp;
	
	printf("	<schedule score=\"%f\">\n", score);
	for(it=s.st_courses_begin(); it!=s.st_courses_end(); it++) {
		// If has theorical class
		if((*it)->theory_group) {
			th_grp = (*it)->theory_group->name();
		}
		else {
			th_grp="";
		}
		// If has lab class
		if((*it)->lab_group) {
			lab_grp = (*it)->lab_group->name();
		}
		else {
			lab_grp = "";
		}
		printf("		<course symbol=\"%s\" theory_grp=\"%s\" lab_group=\"%s\" />\n", (*it)->course->symbol().c_str(), th_grp.c_str(), lab_grp.c_str());
	}
	printf("	</schedule>\n");
}

/* ------------------------------------------------------------------

	Function: usage
	Description: Outputs terse usage information about the program
	Parameters: none
	Return value: none
	Notes: none

------------------------------------------------------------------ */

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
	"  -h, --help - print this help screen and exit\n"
	"  --version - print version\n"
	"  --configfile <config_file> - specify configuration file explicitly\n"
	"  --coursefile <course_file> - specify course file explicitly\n"
	"  --closedfile <closed_file> - specify closed groups file explicitly\n"
	"\n"
	"Action options:\n"
	"  * make\n"
	"    -c <course> - add this course to the schedule\n"
	"                  (repeat this option for each course)\n"
	"    -J <objective> - order results by criteria\n"
	"                     (minholes, maxmorningsleep, maxfreedays)\n"
	"    -T <constraint> - use schedule constraint ()\n"
	"    -t <argument> - specify the argument for the next schedule constraint\n"
	"    -G <constraint> - use group constraint\n"
	"                      (noclosed, noperiod, explicitopen, notbetween)\n"
	"    -g <argument> - specify the argument for the next group constraint\n"
	"\n"
	"Group constraints:\n"
	"  * noclosed\n"
	"  * noperiod\n"
	"  * explicitopen\n"
	"  * notbetween\n"
	"\n";
	
	fprintf(stderr, usage_text);
}


/* ------------------------------------------------------------------

	Function: test_groups
	Description: finds the groups that respect a list of group
		constraints among all the groups in the school's schedule
	Parameters: - requested_courses, the list of courses symbols
		requested by the student
		- schoolschedule, the list of all courses offered by the school
		- goup_constraints, the list of group constraints as objects
		- accepted_groups (out), the groups that match the constraints
		  are added to this set
	Return value: the list of groups that match the constraints is
		returned through the parameter "accepted_groups"
	Notes: none

------------------------------------------------------------------ */

inline void test_groups(vector<string> *requested_courses, SchoolSchedule *schoolschedule, vector<GroupConstraint *> *group_constraints, set<Group *> *accepted_groups)
{
	vector<string>::const_iterator it;
	SchoolCourse::group_list_t::const_iterator it2;
	vector<GroupConstraint *>::const_iterator it3;
	
	// For all courses
	for(it=requested_courses->begin(); it!=requested_courses->end(); it++) {
		// For all groups
		for(it2=schoolschedule->course(*it)->groups_begin(); it2!=schoolschedule->course(*it)->groups_end(); it2++) {
			int success=1;
			// For all constraints
			for(it3=group_constraints->begin(); it3!=group_constraints->end(); it3++) {
				if((**it3)(*it2, schoolschedule->course(*it)) == false) {
					success=0;
					break;
				}
			}
			if(success) {
				accepted_groups->insert(*it2);
			}
		}
	}
}

/* ------------------------------------------------------------------

	Function: test_and_recurse
	Description: When building a schedule with make_recurse, verify 
		that a student schedule meets the constraints before 
		continuing with the recursion
	Parameters: - ss, current schedule to add the remaining_courses
		to
		- remaining_courses, the courses that still have to be added
		to the current schedule
		- constraints, the list of constraint as objets
		- accepted_groups, the list of groups available to build the
		schedule, this is likely obtained from the function
		test_groups
		- solutions, (out) the resulting schedules
	Return value: true if a descendant was found, false otherwise
	Notes: none

------------------------------------------------------------------ */

inline bool test_and_recurse(SchoolSchedule &schoolsched, StudentSchedule ss, vector<string> remaining_courses, vector<string> remaining_opt_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions, bool dontaddasis)
{
	vector<Constraint *>::iterator it;
	for(it=constraints->begin(); it!=constraints->end(); it++) {
		if(!((**it)(ss))) {
			//debug("constraint failed");
			return false;
		}
	}
	
	// If we get here, all the constraints were satisfied, therefore, proceed with the recursion
	make_recurse(schoolsched, ss, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, dontaddasis);
	return true;
}

/* ------------------------------------------------------------------

	Function: make_recurse
	Description: Return all schedules that can contain the
		"remaining_courses" chosen among the "accepted_groups" and
		added to the schedule "ss" while respecting the requested
		"constraints"
	Parameters: - ss, current schedule to add the remaining_courses
		to
		- remaining_courses, the courses that still have to be added
		to the current schedule
		- constraints, the list of constraint as objets
		- accepted_groups, the list of groups available to build the
		schedule, this is likely obtained from the function
		test_groups
		- solutions, (out) the resulting schedules
	Return value: the resulting schedules are returned through the
		parameter "solutions"
	Notes: none

------------------------------------------------------------------ */

void make_recurse(SchoolSchedule &schoolsched, StudentSchedule ss, vector<string> remaining_courses, vector<string> remaining_opt_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions, bool dontaddasis)
{
	SchoolCourse *course_to_add;
	bool course_opt = false;

	if(remaining_courses.size() == 0 && remaining_opt_courses.size() == 0) {
		if(!dontaddasis) {
			solutions.push_back(ss);
		}
		return;
	}
	
	if(remaining_courses.size()) {
		course_to_add = schoolsched.course(remaining_courses.back());
		remaining_courses.pop_back();
	}
	else {
		course_to_add = schoolsched.course(remaining_opt_courses.back());
		remaining_opt_courses.pop_back();
		course_opt = true;
	}

	StudentCourse newcourse;
	newcourse.course = course_to_add;
	
	SchoolCourse::group_list_t::const_iterator it,it2;
	bool found_descendant = false;
	
	if(course_to_add->type() == COURSE_TYPE_THEORYONLY){
		// This course has only theorical sessions
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end()) {
					continue;
				}
					
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				newcourse.lab_group = 0;
				tmps.add_st_course(newcourse);
				found_descendant |= test_and_recurse(schoolsched, tmps, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, false);
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_LABONLY){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if((*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end()) {
					continue;
				}
				
				StudentSchedule tmps(ss);
				newcourse.theory_group = 0;
				newcourse.lab_group = (*it);
				tmps.add_st_course(newcourse);
				found_descendant |= test_and_recurse(schoolsched, tmps, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, false);
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_THEORYLABIND){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end()) {
					continue;
				}
				
				newcourse.theory_group = (*it);
				
				// Try all labs
				
				for(it2=course_to_add->groups_begin(); it2!=course_to_add->groups_end(); it2++) {
					if((*it2)->lab()) {
						if(accepted_groups->find(*it2) == accepted_groups->end()) {
							continue;
						}
						
						StudentSchedule tmps(ss);
						newcourse.lab_group = (*it2);
						tmps.add_st_course(newcourse);
						found_descendant |= test_and_recurse(schoolsched, tmps, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, false);
					}
				}
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_THEORYLABSAME){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end()) {
					continue;
				}
				if(accepted_groups->find(course_to_add->group((*it)->name(), true)) == accepted_groups->end()) {
					continue;
				}
				
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				if(!course_to_add->group_exists((*it)->name(), true)) {
					error("a lab group does not exist for a course with same-lab-group policy");
				}
				
				newcourse.lab_group = course_to_add->group((*it)->name(), true);
				tmps.add_st_course(newcourse);
				found_descendant |= test_and_recurse(schoolsched, tmps, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, false);
			}
		}
	}

	if(course_opt) {
		test_and_recurse(schoolsched, ss, remaining_courses, remaining_opt_courses, constraints, accepted_groups, solutions, 0);
	}
}


/* --------------------------------------------------------------------------

	Function: make
	Description: find all schedules obeying the given conditions and
		output them
	Parameters: pointers to argc & *argv[], the command line argument
		count and values, refer to the function "usage" to know what
		the arguments should be
	Return value: none
	Notes: implements the model of an "action"

-------------------------------------------------------------------------- */

void make(int argc, char **argv)
{
	vector<string> requested_courses;
	vector<string> optional_courses;
	vector<Constraint *> constraints;
	vector<GroupConstraint *> group_constraints;
	set<Group *> accepted_groups;
	int max_scheds=10;
	NullObjective null_obj;
	Objective *objective=&null_obj;
	string next_constraint_arg;
	string next_group_constraint_arg;
	string max_conflicts_str="0";
	long long db_begin;
	long long db_end;
	long long compute_begin;
	long long compute_end;
	
	int c,i;
	
	opterr = 0;
	optind=1;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"course", 1, 0, 'c'},
			{"copt", 1, 0, 'o'},
			{"count", 1, 0, 'n'},
			{"objective", 1, 0, 'J'},
			{"objective-args", 1, 0, 'j'},
			{"constraint", 1, 0, 'T'},
			{"constraint-args", 1, 0, 't'},
			{"max-conflicts", 1, 0, 'C'},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (argc, argv, "c:n:J:j:T:t:G:g:",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
			case 'c':
				/* TODO: output error in xml */
				/*
				if(!schoolsched.course_exists(optarg)) {
					if(output_fmt == OUTPUT_HTML) {
						error("<div class=\"errormsg\"><p>Ce cours n'existe pas: %s\n<p>Pour changer les paramètres, utiliser le bouton Précédent de votre navigateur.</div>", optarg);
					}
					else {
						error("course does not exist: %s", optarg);
					}
				}
				*/
				requested_courses.push_back(optarg);
				break;

			case 'o':
				/* TODO: output error in xml if course not exists */
				optional_courses.push_back(optarg);
				
			case 'n':
				max_scheds=atoi(optarg);
				break;
				
			case 'J':
				if(!strcmp(optarg, "minholes")) {
					objective = new MinHoles();
				}
				else if(!strcmp(optarg, "maxmorningsleep")) {
					objective = new MaxMorningSleep();
				}
				else if(!strcmp(optarg, "maxfreedays")) {
					objective = new MaxFreeDays();
				}
				else if(!strcmp(optarg, "maxcourses")) {
					objective = new MaxCourses();
				}
				else {
					error("invalid objective");
				}
				break;
				
			case 'j':
				break;
				
			case 'T':
				error("unknown constaint (%s)", optarg);
				break;
				
			case 't':
				next_constraint_arg=optarg;
				break;
				
			case 'G':
				if(!strcmp(optarg, "noclosed")) {
					group_constraints.push_back(new NoClosed(next_group_constraint_arg));
				}
				else if(!strcmp(optarg, "noperiod")) {
					group_constraints.push_back(new NoPeriod(next_group_constraint_arg));
				}
				else if(!strcmp(optarg, "explicitopen")) {
					group_constraints.push_back(new ExplicitOpen(next_group_constraint_arg));
				}
				else if(!strcmp(optarg, "notbetween")) {
					group_constraints.push_back(new NotBetween(next_group_constraint_arg));
				}
				else {
					error("unknown constaint (%s)", optarg);
				}
				break;
				
			case 'g':
				next_group_constraint_arg=optarg;
				break;
				
			case 'C':
				max_conflicts_str = optarg;
				break;
	
			case '?':
				error("invalid command line");
				break;
		
			default:
				error("getopt returned unknown code %d\n", c);
		}
	}
	
	// Get a school schedule with the courses we want
	DBLoader dbl;
	SchoolSchedule *schoolsched;
	vector<string> tmp_course_list;
	tmp_course_list = requested_courses;
	tmp_course_list.insert(tmp_course_list.end(), optional_courses.begin(), optional_courses.end());
	db_begin=uepoch();
	schoolsched = dbl.get_ss_with_courses(&tmp_course_list);
	db_end=uepoch();
	
	// Find acceptable groups
	compute_begin=uepoch();
	test_groups(&requested_courses, schoolsched, &group_constraints, &accepted_groups);
	test_groups(&optional_courses, schoolsched, &group_constraints, &accepted_groups);
	
	// Make an empty schedule to begin the recursion with
	StudentSchedule sched;
	vector<StudentSchedule> solutions;
	
	// Prepare constraints
	NoConflicts noc(max_conflicts_str);
	constraints.push_back(&noc);
	
	// Find the schedules that respect the objectives and constraints among all possible schedules
	make_recurse(*schoolsched, sched, requested_courses, optional_courses, &constraints, &accepted_groups, solutions, false);
	
	multimap<float, StudentSchedule *> scores;
	vector<StudentSchedule>::iterator it;
	multimap<float, StudentSchedule *>::const_iterator it2;
	
	// Order the solutions by score
	for(it=solutions.begin(); it!=solutions.end(); it++) {
		scores.insert(pair<float, StudentSchedule *>(objective->operator()(&*it), &*it));
	}
	compute_end=uepoch();
	
	// Print the solutions in order
	printf("<sched_group engine_version=\"" VERSION "\" compute_time=\"%f\" db_time=\"%f\">\n", static_cast<float>(compute_end-compute_begin)/1000000.0, static_cast<float>(db_end-db_begin)/1000000.0);
	for(it2=scores.begin(), i=1; it2!=scores.end(); it2++, i++) {
		print_schedule_xml_groupdefs(*(it2->second), it2->first);
	}
	printf("</sched_group>\n");
}


/* ------------------------------------------------------------------

	Function: to_variable_name
	Description: performs the equivalent of the regular expression
		s/[^a-zA-Z0-9]/_/, that is, it replaces non alphanumeric
		characters with underscores
	Parameters: - s, the initial string with characters to be 
		replaced
	Return value: the string with the replaced characters
	Notes: none

------------------------------------------------------------------ */

string to_variable_name(string s)
{
	string retval = s;
	int i;
	
	for(i=s.size()-1; i>=0; i--) {
		if(!(retval[i]>='a' && retval[i] <= 'z' || retval[i]>='A' && retval[i] <= 'Z' || retval[i]>='0' && retval[i] <= '9')) {
			retval[i] = '_';
		}
	}
	
	return retval;
}


/* ------------------------------------------------------------------

	Function: poly_period_to_time
	Description: Returns the hour in the day of a period number in
		the week
	Parameters: - period, an integer representing the period number 
		in the week
	Return value: an integer representing the time during the day in
		the form hour * 100 + minutes
	Notes: none

------------------------------------------------------------------ */

int poly_period_to_time(int period)
{
	return period%10000;
}


/* ------------------------------------------------------------------

	Function: set_default_options
	Description: Set the default options
	Parameters: none
	Return value: none
	Notes: none

------------------------------------------------------------------ */

void set_default_options()
{
	config_file = "sopti.conf";
}


/* ------------------------------------------------------------------

	Function: parse_command_line
	Description: extract the relevant arguments from the command line
	Parameters: pointers to argc & *argv[], the command line argument
		count and values
	Return value: none
	Notes: modifies the global variable "action", arguments relating
		to an action are not parsed by this function but by the
		action itself

------------------------------------------------------------------ */

void parse_command_line(int *argc, char ***argv)
{
	int c;
	
	opterr = 0;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 5},
			{"coursefile", required_argument, 0, 'c'},
			{"closedfile", required_argument, 0, 4},
			{"configfile", required_argument, 0, 6},
			{"html", 0, 0, 3},
			{0, 0, 0, 0}
		};
	
		/* + indicates to stop at the first non-argument option */
		c = getopt_long (*argc, *argv, "+hvc:", long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
	
		case 'c':
			course_file=optarg;
			break;
		
		case 'h':
			usage();
			exit(0);
		case 3:
			output_fmt = OUTPUT_HTML;
			break;

		case 4:
			closedgroups_file=optarg;
			break;
			
		case 5:
			printf("%s\n", PACKAGE_STRING);
			exit(0);
			
		case 6:
			config_file=optarg;
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


/* ------------------------------------------------------------------

	Function: parse_config_file
	Description: Read the configuration files and assign option values
	Parameters: conffile_name, a string representing the
		configuration file's name
	Return value: none
	Notes: this is a work in progress, there's currently no support
		for configuration files

------------------------------------------------------------------ */

/*
void parse_config_file(string conffile_name)
{
	try {
		config_vars = parse_config_file(conffile_name);
	}
	catch {
		error();
	}
}
*/

/* ------------------------------------------------------------------

	Function: main
	Description: entry function of the program
	Parameters: argc & *argv[], the command line argument
		count and values
	Return value: 0 on success, 1 on error
	Notes: none

------------------------------------------------------------------ */

int main(int argc, char **argv)
{
	int i;
	
	set_default_options();
	parse_command_line(&argc, &argv);
	parse_config_file(config_file);

	stdsched_init();
	
	//load_courses_from_csv(&schoolsched, course_file);
	//load_closed_from_csv(&schoolsched, closedgroups_file);
		
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
