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

#include "schoolschedule.hpp"

using namespace std;


/* ------------------------------------------------------------------

	Function: SchoolSchedule
	Description: empty default constructor
	Parameters: none
	Return value: none
	Notes: none

------------------------------------------------------------------ */

SchoolSchedule::SchoolSchedule()
{
}

SchoolSchedule::~SchoolSchedule()
{
	course_list_t::const_iterator it;
	for(it = courses_begin(); it != courses_end(); it++) {
		delete *it;
	}
}

/* ------------------------------------------------------------------

	Function: course_exists
	Description: returns wether a course exists or not based on the
		course's symbol
	Parameters: c, the course's symbol
	Return value: boolean, true if the course exists in the school's
		schedule
	Notes: none

------------------------------------------------------------------ */

bool SchoolSchedule::course_exists(string c)
{
	return p_courses_order_sym.find(c) != p_courses_order_sym.end();
}


/* ------------------------------------------------------------------

	Function: course_add
	Description: add a course to the school's schedule
	Parameters: c, a SchoolCourse object
	Return value: none
	Notes: the function add_course should be prefered over this one

------------------------------------------------------------------ */

void SchoolSchedule::course_add(SchoolCourse c)
{
	SchoolCourse *tmp = new SchoolCourse(c);
	
	// Add to the structures
	p_courses.push_back(tmp);
	p_courses_order_sym[tmp->symbol()] = tmp;
}


/* ------------------------------------------------------------------

	Function: add_course
	Description: add a course to the school's schedule
	Parameters: c, a pointer to a SchoolCourse object
	Return value: none
	Notes: c must not be deallocated afterwards

------------------------------------------------------------------ */

void SchoolSchedule::add_course(SchoolCourse *c)
{
	// Add to the structures
	p_courses.push_back(c);
	p_courses_order_sym[c->symbol()] = c;
}


/* ------------------------------------------------------------------

	Function: course
	Description: get a pointer to the SchoolCourse object of a 
		certain course
	Parameters: c, the desired course's symbol
	Return value: the SchoolCourse object that has the symbol c
	Notes: the returned pointer must not be deallocated

------------------------------------------------------------------ */

SchoolCourse *SchoolSchedule::course(std::string c)
{
	if(!course_exists(c))
		return 0;
		
	return p_courses_order_sym[c];
}
