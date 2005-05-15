#ifndef SCHOOLSCHEDULE_HPP
#define SCHOOLSCHEDULE_HPP

#include <map>
#include <vector>
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

#include "schoolcourse.hpp"

class CourseSymOrder
{
	public:
	CourseSymOrder() {}
	bool operator()(const SchoolCourse *a, const SchoolCourse *b) {
		return a->symbol() < b->symbol();
	}
};

/* Class that constains all Courses offered at the
   school */

class SchoolSchedule
{
	public:

	typedef std::vector<SchoolCourse *> course_list_t;

	SchoolSchedule();
	SchoolCourse *course(std::string);
	
	bool course_exists(std::string);
	void course_add(SchoolCourse); // deprecated
	void add_course(SchoolCourse *);
	course_list_t::const_iterator courses_begin() { return p_courses.begin(); }
	course_list_t::const_iterator courses_end() { return p_courses.end(); }
	
	private:
	// The courses, no particular order
	std::vector<SchoolCourse *> p_courses;
	// The courses, ordered by symbol
	std::map<std::string, SchoolCourse *> p_courses_order_sym;
};

#endif // SCHOOLSCHEDULE_HPP
