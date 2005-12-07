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

#include "studentschedule.hpp"

StudentSchedule::StudentSchedule(const StudentSchedule &s)
{
	// Copy everything
	*this = s;
	
	// Make copies of the courses
	course_list_t::iterator it;
	for(it = p_courses.begin(); it != p_courses.end(); it++) {
		*it = new struct StudentCourse(**it);
	}
}

StudentSchedule::~StudentSchedule()
{
	course_list_t::const_iterator it;
	for(it = st_courses_begin(); it != st_courses_end(); it++) {
		delete *it;
	}
}

/* ------------------------------------------------------------------

	Function:add_st_course
	Description: add a course to a student schedule
	Parameters: g, the course to add as a StudentCourse object
	Return value: none
	Notes: none

------------------------------------------------------------------ */

void StudentSchedule::add_st_course(StudentCourse g)
{
	StudentCourse *tmp;
	
	tmp = new StudentCourse(g);
	p_courses.push_back(tmp);
}
