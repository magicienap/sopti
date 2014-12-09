#include <string>
#include <fstream>

#include <stdlib.h>

#include "read_csv.hpp"
#include "globals.hpp"
#include "schoolschedule.hpp"
#include "schoolcourse.hpp"
#include "group.hpp"
#include "period.hpp"

using namespace std;


/* ------------------------------------------------------------------

	Function: split_string
	Description: split a string on boundaries formed by a separator
		into a table of substrings
	Parameters: - s, the input string
		- sep, the separator
	Return value: an stl vector filled with the substrings
	Notes: none

------------------------------------------------------------------ */

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


/* ------------------------------------------------------------------

	Function: poly_make_period_no
	Description: return a period number to use, for example, in a 
		Period struct, identifying a certain period in the week by 
		its	day and time
	Parameters: - day_of_week, the first three characters of the
		day's name in french and capitalized
		- time_str, the period's starting hour in the form hour * 100
		+ minutes
	Return value: the integer representing the period number
	Notes: none

------------------------------------------------------------------ */

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


/* ------------------------------------------------------------------

	Function: load_courses_from_csv
	Description: read the csv file describing all the courses given
		by the school and store them in a SchoolSchedule object
	Parameters: - sopti, (out) a pointer to the SchoolSchedule object
		in which to store the courses
		- periods_file, the filename of the csv file
	Return value: none
	Notes: this function depends on the macros defined in the
		matching header file for the field orders

------------------------------------------------------------------ */

void load_courses_from_csv(SchoolSchedule *sopti, string periods_file)
{
	ifstream period_list;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	// Open the file
	period_list.open(periods_file.c_str());
	if(period_list.fail()) {
		error("error opening file %s", periods_file.c_str());
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
		SchoolCourse *current_course;
		
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
		
		current_course = sopti->course(fields[COURSEFILE_FIELD_SYMBOL]);
		
		// Add group if not exists
		if(!current_course->group_exists(fields[COURSEFILE_FIELD_GROUP], islab)) {
			Group newgroup(fields[COURSEFILE_FIELD_GROUP]);
			newgroup.set_lab(islab);
			
			current_course->add_group(newgroup, islab);
		}
		
		// Add period. It should not exist.
		
		int period_no = poly_make_period_no(fields[COURSEFILE_FIELD_DAY], fields[COURSEFILE_FIELD_TIME]);
		if(current_course->group(fields[COURSEFILE_FIELD_GROUP], islab)->has_period(period_no)) {
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
		current_course->group(fields[COURSEFILE_FIELD_GROUP], islab)->add_period(newperiod);
	}
	
	period_list.close();
}


/* ------------------------------------------------------------------

	Function: load_closed_from_csv
	Description: read the csv file describing the closed groups, that
		is, the groups in which no more space is available to
		register and add this information in a pre-filled
		SchoolSchedule object
	Parameters: - sopti, (out) a pointer to the SchoolSchedule object
		in which to update the information about the closed groups
		- closed_file, the filename of the csv file
	Return value: none
	Notes: this function depends on the macros defined in the
		matching header file for the field orders

------------------------------------------------------------------ */

void load_closed_from_csv(SchoolSchedule *sopti,  string closed_file)
{
	ifstream  closed_list;
	string tmps;
	char tmpa[500];
	vector<string> fields;
	
	
	
	closed_list.open(closed_file.c_str());
	if(closed_list.fail()) {
		error("error opening file %s", closed_file.c_str());
	}
	
	// Ignore header line
	closed_list.getline(tmpa, 500);
	
	
	while(1) {
		closed_list.getline(tmpa, 500);
		if(!closed_list.good()) {
			break;
		}
		
		tmps = tmpa;
		if(tmps[tmps.size()-1] == '\r')
			tmps.erase(tmps.size()-1, 1);
	
		fields = split_string(tmps, ";");
		if(fields.size() != 6) {
			error("invalid closed group file format (field_num != 6)");
		}
		
		// prepare some variables in advance
		bool islab;
		
				
		if(fields[CLOSEDFILE_FIELD_COURSELAB] == "L")
			islab=true;
		else if(fields[CLOSEDFILE_FIELD_COURSELAB] == "C")
			islab=false;
		else
			error("unrecognized course type");
		
		sopti->course(fields[CLOSEDFILE_FIELD_SYMBOL])->group(fields[CLOSEDFILE_FIELD_GROUP], islab)->set_closed(true);

	}
	
	closed_list.close();
}
