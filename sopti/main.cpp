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
	vector<SchoolCourse *>::iterator it;
	vector<SchoolCourse *> l_courses;
	SchoolCourse::group_list_t::const_iterator it2;
	
	// Sort the courses before printing them
	l_courses.insert(l_courses.end(), schoolsched.courses_begin(), schoolsched.courses_end());
	sort<vector<SchoolCourse *>::iterator, SchoolCoursePtrSymAlphaOrder>(l_courses.begin(), l_courses.end(), SchoolCoursePtrSymAlphaOrder());

	for(it=l_courses.begin(); it!=l_courses.end(); it++) {
		cout << (*it)->symbol() << " " << (*it)->title();
		for(it2=(*it)->groups_begin(); it2!=(*it)->groups_end(); it2++) {
			cout << " " << (*it2)->name() << "(" << ((*it2)->lab()?"L":"T") << ")" << ((*it2)->closed()?"*":"");
		}
		cout << endl;
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


/* ------------------------------------------------------------------

	Function: print_schedule_html
	Description: Outputs a schedule in html form
	Parameters: s, the schedule to output
	Return value: none
	Notes: none

------------------------------------------------------------------ */

void print_schedule_html(StudentSchedule &s)
{
	int i,j;
	
	// sched_standard[week][day_of_week][period_id][sequential]
	map< int, vector<ScheduledPeriod> > sched_standard[3][7];
	// sched_nonstandard[week][day_of_week][hour][sequential]
	map< int, vector< ScheduledPeriod > > sched_nonstandard[3][7];
	
	StudentSchedule::course_list_t::const_iterator it;
	Group::period_list_t::const_iterator it2;
	map<int, int>::const_iterator it3;
	vector<ScheduledPeriod>::const_iterator it4;
	map< int, vector< ScheduledPeriod > >::const_iterator it5;
	
	for(it=s.st_courses_begin(); it!=s.st_courses_end(); it++) {
		// If has theorical class
		if((*it)->theory_group) {
			for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
				int day_of_week = (*it2)->period_no()/10000-1;
				int week = (*it2)->week();
				int hour = (*it2)->period_no()%((day_of_week+1)*10000);
				ScheduledPeriod schedperiod;
				
				schedperiod.course = (*it)->course;
				schedperiod.group = (*it)->theory_group;
				schedperiod.period = (*it2);
				
				it3 = stdsched_hour2id[stdsched_day2template[day_of_week]].find(hour);
				if(it3 == stdsched_hour2id[stdsched_day2template[day_of_week]].end()) {
					sched_nonstandard[week][day_of_week][hour].push_back(schedperiod);
				}
				else {
					sched_standard[week][day_of_week][(*it3).second].push_back(schedperiod);
				}
			}
		}
		// If has lab class
		if((*it)->lab_group) {
			for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
				int day_of_week = (*it2)->period_no()/10000-1;
				int week = (*it2)->week();
				int hour = (*it2)->period_no()%((day_of_week+1)*10000);
				ScheduledPeriod schedperiod;
				
				schedperiod.course = (*it)->course;
				schedperiod.group = (*it)->lab_group;
				schedperiod.period = (*it2);
				
				it3 = stdsched_hour2id[stdsched_day2template[day_of_week]].find(hour);
				if(it3 == stdsched_hour2id[stdsched_day2template[day_of_week]].end()) {
					sched_nonstandard[week][day_of_week][hour].push_back(schedperiod);
				}
				else {
					sched_standard[week][day_of_week][(*it3).second].push_back(schedperiod);
				}
			}
		}
	}
	
	printf("<table class=\"schedule\">\n");
	
	// Don't forget the empty column for the hours
	printf("<tr><td></td>\n");
	
	for(i=0; i<5; i++) {
		printf("<td class=\"weekday\">%s</td>", stdsched_day2name[i]);
	}
	
	printf("</tr>\n");

	bool week_finished=false;
	bool weekend_finished=false;
	
	// For each hour
	for(i=0;; i++) {
	
		if(!week_finished && stdsched_daytemplates[0][i].start == -1)
			week_finished = true;
			
		if(week_finished)
			break;
	
		printf("<tr>\n");
		// Print hour
		printf("<td class=\"hour\">");
		printf("<b>%d:%.2d</b><br>", stdsched_daytemplates[0][i].start/100, stdsched_daytemplates[0][i].start%100);

		printf("</td>\n");
		
		for(j=0; j<5; j++) {
			printf("<td>");
			
			// Determine the conflict scheme
			int conflict_mask=0;
			if(sched_standard[0][j][i].size() > 1) {
				// More than one weekly course
				conflict_mask = ~0;
			}
			else if(sched_standard[0][j][i].size() == 1 && (sched_standard[1][j][i].size() > 0 || sched_standard[2][j][i].size() > 0)) {
				conflict_mask = ~0;
			}
			else if(sched_standard[0][j][i].size() == 0) {
				if(sched_standard[1][j][i].size() > 1) {
					conflict_mask |= 1;
				}
				if(sched_standard[2][j][i].size() > 1) {
					conflict_mask |= 2;
				}
			}
			
			// Now print this period
			for(it4=sched_standard[0][j][i].begin(); it4!=sched_standard[0][j][i].end(); it4++) {
				printf("<div class=\"%s\"><b>%s</b> (%s)<br>%s (%s)</div>\n", conflict_mask?"period_conflict":"period_noconflict", it4->course->symbol().c_str(), it4->group->lab()?"Lab":"Th", it4->period->room().c_str(), it4->group->name().c_str());
			}
			for(it4=sched_standard[1][j][i].begin(); it4!=sched_standard[1][j][i].end(); it4++) {
				printf("<div class=\"%s\"><b>%s</b> (%s)<br>%s (%s) B%d</div>\n", (conflict_mask&(1<<(it4->period->week()-1)))?"period_conflict":"period_noconflict", it4->course->symbol().c_str(),it4->group->lab()?"Lab":"Th", it4->period->room().c_str(),it4->group->name().c_str(), it4->period->week());
			}
			for(it4=sched_standard[2][j][i].begin(); it4!=sched_standard[2][j][i].end(); it4++) {
				printf("<div class=\"%s\"><b>%s</b> (%s)<br>%s (%s) B%d</div>\n", (conflict_mask&(1<<(it4->period->week()-1)))?"period_conflict":"period_noconflict", it4->course->symbol().c_str(),it4->group->lab()?"Lab":"Th", it4->period->room().c_str(),it4->group->name().c_str(), it4->period->week());
			}
			
			printf("</td>");
		}
		printf("</tr>\n");
	}

	printf("<tr><td class=\"hour\">Heures non standard</td>");
	for(j=0; j<5; j++) {
		printf("<td class=\"period\">");
		for(it5=sched_nonstandard[0][j].begin(); it5!=sched_nonstandard[0][j].end(); it5++) {
			for(it4=it5->second.begin(); it4!=it5->second.end(); it4++) {
				printf("<div class=\"%s\">%d:%.2d<br><b>%s</b> (%s)<br>%s (%s)</div>\n", "period_noconflict", (it5->first)/100, (it5->first)%100, it4->course->symbol().c_str(), it4->group->lab()?"Lab":"Th", it4->period->room().c_str(), it4->group->name().c_str());
			}
		}
		printf("</td>");
		/*
		for(it4=sched_standard[1][j][i].begin(); it4!=sched_standard[1][j][i].end(); it4++) {
			printf("<tr><td class=\"%s\"><b>%s</b> (%s)<br>%s (%s) B%d</tr></td>\n", (conflict_mask&(1<<(it4->period->week()-1)))?"period_conflict":"period_noconflict", it4->course->symbol().c_str(),it4->group->lab()?"Lab":"Th", it4->period->room().c_str(),it4->group->name().c_str(), it4->period->week());
		}
		for(it4=sched_standard[2][j][i].begin(); it4!=sched_standard[2][j][i].end(); it4++) {
			printf("<tr><td class=\"%s\"><b>%s</b> (%s)<br>%s (%s) B%d</tr></td>\n", (conflict_mask&(1<<(it4->period->week()-1)))?"period_conflict":"period_noconflict", it4->course->symbol().c_str(),it4->group->lab()?"Lab":"Th", it4->period->room().c_str(),it4->group->name().c_str(), it4->period->week());
		}
		*/
	}
	printf("</tr>\n");
	
	printf("</table>\n");
	
	// PRINT WEEKEND SCHEDULE
/*
	if(!sched[5].empty() || !sched[6].empty()) {
		// print only if we have weekend courses
		
		printf("<table class=\"schedule_weekend\">\n");
		
		// Don't forget the empty column for the hours
		printf("<tr><td></td>\n");
		
		for(i=5; i<7; i++) {
			printf("<td class=\"weekday\">%s</td>", days_of_week[i]);
		}
		
		printf("</tr>\n");
	
		// For each hour
		for(i=0;; i++) {
			if(!weekend_finished && hours_weekend[i] == -1)
				weekend_finished = true;
			
			if(weekend_finished)
				break;
		
			printf("<tr>\n");
			// Print hour
			printf("<td class=\"hour\">");
			printf("<b>%d:%.2d</b><br>", hours_weekend[i]/100, hours_weekend[i]%100);
	
			printf("</td>\n");
			
			for(j=5; j<7; j++) {
				printf("<td class=\"period\">");
				if(sched[j].find(hours_weekend[i]) != sched[j].end()) {
				
					// If there is only one course
					if(sched[j][hours_weekend[i]].size() == 1) {
						printf("%s", sched[j][hours_weekend[i]][0].c_str());
					}
					else {
						unsigned int k;
						printf("<table class=\"conflict_table\">\n");
						for(k=0; k<sched[j][hours_weekend[i]].size(); k++) {
							printf("<tr><td>%s</td></tr>\n", sched[j][hours_weekend[i]][k].c_str());
						}
						printf("</table>\n");
					}
				}
				else {
					printf("&nbsp;");
				}
				printf("</td>");
			}
			printf("</tr>\n");
		}
	
		printf("</table>\n");
	}
	*/
}


/* ------------------------------------------------------------------

	Function: print_schedule
	Description: Outputs a schedule
	Parameters: s, the schedule to output
	Return value: none
	Notes: the format of the output will depend on the global
		variable "output_fmt"

------------------------------------------------------------------ */

void print_schedule(StudentSchedule &s)
{
	if(output_fmt == OUTPUT_HTML)
		print_schedule_html(s);
	else
		print_schedule_ascii(s);
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
	"  get_open_close_form - print an html form to choose the groups to open\n"
	"  listcourses - print a list of courses\n"
	"  make - make schedules\n"
	"\n"
	"Global options:\n"
	"  --coursefile <course_file> - specify course file explicitly\n"
	"  --closedfile <closed_file> - specify closed groups file explicitly\n"
	"  --html - enable html output\n"
	"\n"
	"Action options:\n"
	"  * get_open_close_form\n"
	"    -c <course> - add this course to the schedule\n"
	"                  (repeat this option for each course)\n"
	"  * make\n"
	"    -c <course> - add this course to the schedule\n"
	"                  (repeat this option for each course)\n"
	"    -J <objective> - order results by criteria (minholes)\n"
	"    -T <constraint> - use schedule constraint (noevening)\n"
	"    -t <argument> - specify the argument for the next schedule constraint\n"
	"    -G <constraint> - use group constraint ()\n"
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
		- accepted_groups (out), the list of groups that match the
		constraints
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
	Return value: none
	Notes: none

------------------------------------------------------------------ */

inline void test_and_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions)
{
	vector<Constraint *>::iterator it;
	for(it=constraints->begin(); it!=constraints->end(); it++) {
		if(!((**it)(ss))) {
			//debug("constraint failed");
			return;
		}
	}
	
	// If we get here, all the constraints were satisfied, therefore, proceed with the recursion
	make_recurse(ss, remaining_courses, constraints, accepted_groups, solutions);
}

/*
inline bool test_group_constraints(Group *group, SchoolCourse *course, vector<GroupConstraint *> *group_constraints)
{
	vector<GroupConstraint *>::const_iterator it;

	for(it=group_constraints->begin(); it!=group_constraints->end(); it++) {
		if(!((**it)(group, course))) {
			return false;
		}
	}
	
	return true;
}
*/


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

void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions)
{
	SchoolCourse *course_to_add;

	if(remaining_courses.size() == 0) {
		solutions.push_back(ss);
		return;
	}
	
	course_to_add = schoolsched.course(remaining_courses.back());
	remaining_courses.pop_back();

	StudentCourse newcourse;
	newcourse.course = course_to_add;
	
	SchoolCourse::group_list_t::const_iterator it,it2;
	
	if(course_to_add->type() == COURSE_TYPE_THEORYONLY){
		// This course has only theorical sessions
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end())
					continue;
					
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				newcourse.lab_group = 0;
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, accepted_groups, solutions);
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_LABONLY){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if((*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end())
					continue;
				
				StudentSchedule tmps(ss);
				newcourse.theory_group = 0;
				newcourse.lab_group = (*it);
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, accepted_groups, solutions);
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_THEORYLABIND){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end())
					continue;
				
				newcourse.theory_group = (*it);
				
				// Try all labs
				
				for(it2=course_to_add->groups_begin(); it2!=course_to_add->groups_end(); it2++) {
					if((*it2)->lab()) {
						if(accepted_groups->find(*it) == accepted_groups->end())
							continue;
						
						StudentSchedule tmps(ss);
						newcourse.lab_group = (*it2);
						tmps.add_st_course(newcourse);
						test_and_recurse(tmps, remaining_courses, constraints, accepted_groups, solutions);
					}
				}
			}
		}
	}
	else if(course_to_add->type() == COURSE_TYPE_THEORYLABSAME){
		for(it=course_to_add->groups_begin(); it!=course_to_add->groups_end(); it++) {
			if(!(*it)->lab()) {
				if(accepted_groups->find(*it) == accepted_groups->end())
					continue;
				if(accepted_groups->find(course_to_add->group((*it)->name(), true)) == accepted_groups->end())
					continue;
				
				StudentSchedule tmps(ss);
				newcourse.theory_group = (*it);
				if(!course_to_add->group_exists((*it)->name(), true)) {
					error("a lab group does not exist for a course with same-lab-group policy");
				}
				
				newcourse.lab_group = course_to_add->group((*it)->name(), true);
				tmps.add_st_course(newcourse);
				test_and_recurse(tmps, remaining_courses, constraints, accepted_groups, solutions);
			}
		}
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
	vector<Constraint *> constraints;
	vector<GroupConstraint *> group_constraints;
	set<Group *> accepted_groups;
	int max_scheds=10;
	NullObjective null_obj;
	Objective *objective=&null_obj;
	string next_constraint_arg;
	string next_group_constraint_arg;
	string max_conflicts_str="0";
	
	int c,i;
	
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
				if(!schoolsched.course_exists(optarg)) {
					if(output_fmt == OUTPUT_HTML) {
						error("<div class=\"errormsg\"><p>Ce cours n'existe pas: %s\n<p>Pour changer les paramètres, utiliser le bouton Précédent de votre navigateur.</div>", optarg);
					}
					else {
						error("course does not exist: %s", optarg);
					}
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
				else if(!strcmp(optarg, "maxmorningsleep")) {
					objective = new MaxMorningSleep();
				}
				else if(!strcmp(optarg, "maxfreedays")) {
					objective = new MaxFreeDays();
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
	
	// Find acceptable groups
	test_groups(&requested_courses, &schoolsched, &group_constraints, &accepted_groups);
	
	// Make an empty schedule to begin the recursion with
	StudentSchedule sched;
	vector<StudentSchedule> solutions;
	
	// Prepare constraints
	NoConflicts noc(max_conflicts_str);
	constraints.push_back(&noc);
	
	// Find the schedules that respect the objectives and constraints among all possible schedules
	make_recurse(sched, requested_courses, &constraints, &accepted_groups, solutions);
	
	if(solutions.size() == 0) {
		if(output_fmt == OUTPUT_HTML) {
			printf("<div class=\"errormsg\"><p>Aucune solution trouvée!<p>Causes possibles:<ul><li>Certaines sections sont pleines<li>Les cours sélectionnés entrent en conflit\n</ul><p>Pour changer les paramètres, utiliser le bouton Précédent de votre navigateur.</div>");
		}
		else {
			error("No solution found!");
		}
		return;
	}
	
	multimap<float, StudentSchedule *> scores;
	vector<StudentSchedule>::iterator it;
	multimap<float, StudentSchedule *>::const_iterator it2;
	
	// Order the solutions by score
	for(it=solutions.begin(); it!=solutions.end(); it++) {
		scores.insert(pair<float, StudentSchedule *>(objective->operator()(&*it), &*it));
	}
	
	// Print the summary
	if (output_fmt == OUTPUT_HTML) {
		printf("<div class=\"make_summary\">\n");
		printf("<p>Nombre d'horaires trouvés: %d\n", solutions.size());
		printf("</div>\n");
	}
	else {
		printf("Nombre d'horaires trouvés: %d\n", solutions.size());
	}
	
	// Print the solutions in order
	for(it2=scores.begin(), i=1; it2!=scores.end(); it2++, i++) {
		if(output_fmt == OUTPUT_HTML) {
			printf("<div class=\"make_schedule_info\">\n");
			printf("<p>Horaire #%d\n", i);
			printf("<p>Score: %g\n", it2->first);
			printf("</div>\n");
			// Print group information
			printf("<div class=\"schedule_info\">\n");
			printf("<table class=\"group_summary\">\n");
			printf("<tr><th>Sigle</th><th>Titre</th><th>Théorie</th><th>Lab</th></tr>\n");
			
			StudentSchedule::course_list_t::const_iterator it4;
			for(it4=it2->second->st_courses_begin(); it4!=it2->second->st_courses_end(); it4++) {
				printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", (*it4)->course->symbol().c_str(), (*it4)->course->title().c_str(), (*it4)->theory_group?(*it4)->theory_group->name().c_str():"-", (*it4)->lab_group?(*it4)->lab_group->name().c_str():"-");
			}
			printf("</table>\n");
			printf("</div>\n");
		}
		else {
			printf("Score: %f\n",it2->first);
		}
		print_schedule(*(it2->second));
	}
	//debug("got %d solutions!", solutions.size());
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

	Function: get_open_close_form
	Description: Outputs and html form in a table to override the 
		chosen courses' groups' open/closed status
	Parameters: pointers to argc & *argv[], the command line argument
		count and values, those should include a number of "-c ####"
		where "####" is a course symbol
	Return value: none
	Note: Overriding a group's open/closed status can be convenient
		when, for example, a student is already assigned to a closed
		group or when a student expects that a group is gonna
		become open again because of other students' changes

------------------------------------------------------------------ */

void get_open_close_form(int argc, char **argv)
{
	vector<string> requested_courses;
	int c;
	
	// reset getopt
	opterr = 0;
	optind=1;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"course", 1, 0, 'c'},
			{0, 0, 0, 0}
		};
	
		c = getopt_long (argc, argv, "c:",
				long_options, &option_index);
		if (c == -1)
			break;
	
		switch (c) {
			case 'c':
				/* Verify later if courses exist */
				requested_courses.push_back(optarg);
				break;
	
			case '?':
				error("invalid command line");
				break;
		
			default:
				error("getopt returned unknown code %d\n", c);
		}
	}
	
	vector<string>::const_iterator it;
	SchoolCourse::group_list_t::const_iterator it2;
	string script;
	string openclose_vars;
	
	printf("<table class=\"openclose_table\">\n");
	
	for(it=requested_courses.begin(); it!=requested_courses.end(); it++) {
	
		if(!schoolsched.course_exists(*it)) {
			printf("<tr class=\"openclose_row\">\n<td class=\"course_sym\">%s</td><td colspan=\"3\">Ce cours n'existe pas!</td></tr>", it->c_str());
		}
		else {
			SchoolCourse *current_course;
			int rowspan;
			string script_open, script_close;
			
			// Select number of rows
			current_course = schoolsched.course(*it);
			if(current_course->type() == COURSE_TYPE_THEORYLABIND) {
				rowspan = 3;
			}
			else {
				rowspan = 1;
			}
			
			printf("<tr class=\"openclose_row\">\n<td rowspan=\"%d\" class=\"course_sym\">%s<br>%s</td>", rowspan, current_course->symbol().c_str(), current_course->title().c_str());
			
			if(current_course->type() != COURSE_TYPE_LABONLY) {
				script_open = "function open_all_t_" + to_variable_name(current_course->symbol().c_str()) + "() {\n";
				script_close = "function close_all_t_" + to_variable_name(current_course->symbol().c_str()) + "() {\n";
				
			
				printf("<td>Théorique%s</td>", (current_course->type()==COURSE_TYPE_THEORYLABSAME)?" / lab":"");
				printf("<td>");
				for(it2=current_course->groups_begin(); it2!=current_course->groups_end(); it2++) {
					if((*it2)->lab())
						continue;
						
					script_open += "\tdocument.form2.t_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + ".checked=true;\n";
					script_close += "\tdocument.form2.t_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + ".checked=false;\n";
					openclose_vars+="t_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + " ";
					
					if((*it2)->closed()) {
						printf("<div class=\"full_cbox\"><input name=\"t_%s_%s\" type=\"checkbox\">", to_variable_name(current_course->symbol()).c_str(), to_variable_name((*it2)->name()).c_str());
					}
					else {
						printf("<div class=\"notfull_cbox\"><input name=\"t_%s_%s\" type=\"checkbox\" checked>", to_variable_name(current_course->symbol()).c_str(), to_variable_name((*it2)->name()).c_str());
					}
					
					printf("%s</div>", (*it2)->name().c_str());
				}
				
				script_open += "}\n\n";
				script_close += "}\n\n";
				
				script += script_open;
				script += script_close;
								
				printf("</td><td><input type=\"button\" value=\"Ouvrir toutes\" onClick=\"open_all_t_%s()\"><input type=\"button\" value=\"Fermer toutes\" onClick=\"close_all_t_%s()\"></td>", to_variable_name(current_course->symbol()).c_str(), to_variable_name(current_course->symbol()).c_str());
			}
			if(current_course->type() == COURSE_TYPE_THEORYLABIND) {
				printf("</tr>\n<tr><td colspan=\"4\" style=\"height:1; background-color: white;\"></td></tr>\n<tr>\n");
			}
			if(current_course->type() == COURSE_TYPE_LABONLY || current_course->type() == COURSE_TYPE_THEORYLABIND) {
				script_open = "function open_all_l_" + to_variable_name(current_course->symbol().c_str()) + "() {\n";
				script_close = "function close_all_l_" + to_variable_name(current_course->symbol().c_str()) + "() {\n";
				
				printf("<td>Lab</td>");
				printf("<td>");
				for(it2=current_course->groups_begin(); it2!=current_course->groups_end(); it2++) {
					if(!(*it2)->lab())
						continue;

					script_open += "\tdocument.form2.l_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + ".checked=true;\n";
					script_close += "\tdocument.form2.l_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + ".checked=false;\n";
					openclose_vars += "l_" + to_variable_name(current_course->symbol().c_str()) + "_" + to_variable_name((*it2)->name()) + " ";
					
					if((*it2)->closed()) {
						printf("<div class=\"full_cbox\"><input name=\"l_%s_%s\" type=\"checkbox\">", current_course->symbol().c_str(), (*it2)->name().c_str());
					}
					else {
						printf("<div class=\"notfull_cbox\"><input name=\"l_%s_%s\" type=\"checkbox\" checked>", current_course->symbol().c_str(), (*it2)->name().c_str());
					}
					
					printf("%s</div>", (*it2)->name().c_str());
				}
				
				script_open += "}\n\n";
				script_close += "}\n\n";
				
				script += script_open;
				script += script_close;
				
				printf("</td><td><input type=\"button\" value=\"Ouvrir toutes\" onClick=\"open_all_l_%s()\"><input type=\"button\" value=\"Fermer toutes\" onClick=\"close_all_l_%s()\"></td>", current_course->symbol().c_str(), current_course->symbol().c_str());
			}
			
			printf("</tr>");
		}
		printf("<tr><td colspan=\"4\" class=\"openclose_whitespace\">&nbsp;</td></tr>\n");
	}
	
	printf("</table>\n\n");
	
	printf("<input type=\"hidden\" name=\"openclose_vars\" value=\"%s\">\n\n", openclose_vars.c_str());
	
	printf("<script type=\"text/javascript\">\n%s</script>\n", script.c_str());
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
			{"coursefile", required_argument, 0, 'c'},
			{"closedfile", required_argument, 0, 4},
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

		case 4:
			closedgroups_file=optarg;
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
	//parse_config_file(config_file);
	stdsched_init();
	
	load_courses_from_csv(&schoolsched, course_file);
	load_closed_from_csv(&schoolsched, closedgroups_file);
	
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
