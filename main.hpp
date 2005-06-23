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


#ifndef MAIN_HPP
#define MAIN_HPP

#define OUTPUT_HTML	1

#include <string>
#include <vector>
#include <set>

#include "schoolcourse.hpp"
#include "studentschedule.hpp"
#include "constraint.hpp"
#include "group.hpp"
#include "period.hpp"

using namespace std;

void listcourses(int, char **);
void print_schedule_ascii(StudentSchedule &s);
void print_schedule_html(StudentSchedule &s);
void print_schedule(StudentSchedule &s);
void usage();
// inline void test_groups(vector<string> *requested_courses, SchoolSchedule *schoolschedule, vector<GroupConstraint *> *group_constraints, set<Group *> *accepted_groups)
// inline void test_and_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions)
void make_recurse(StudentSchedule ss, vector<string> remaining_courses, vector<Constraint *> *constraints, set<Group *> *accepted_groups, vector<StudentSchedule> &solutions);
void make(int argc, char **argv);
string to_variable_name(string s);
void get_open_close_form(int argc, char **argv);
int poly_period_to_time(int period);
void set_default_options();
void parse_command_line(int *argc, char ***argv);
void parse_config_file(string conffile_name);
int main(int argc, char **argv);


/* Describes an action that can be passed
 * at command line. The arguments are
 * argc and argv, but only containing the
 * arguments that apply to the action (only
 * those including and after the action name.
 */

struct Action {
	char name[20];
	void (*func)(int, char **);
};


struct Action actions[] =
	{
		{ "listcourses", listcourses},
		{ "make", make},
		{ "get_open_close_form", get_open_close_form},
		{ 0, 0 }
	};

	
struct ScheduledPeriod {
	SchoolCourse *course;
	Group *group;
	Period *period;
};


class SchoolCoursePtrSymAlphaOrder
{
	public:
	SchoolCoursePtrSymAlphaOrder() {}
	bool operator()(const SchoolCourse *a, const SchoolCourse *b) {
		return a->symbol() < b->symbol();
	}
};


string config_file="sopti.conf";
string course_file="data/courses.csv";
string closedgroups_file="data/closed.csv";
string action;
SchoolSchedule schoolsched;
int output_fmt=0;

#endif // MAIN_HPP
