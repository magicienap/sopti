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


#ifndef STUDENTSCHEDULE_HPP
#define STUDENTSCHEDULE_HPP

#include <string>
#include <vector>

#include "schoolcourse.hpp"
#include "group.hpp"

/* Class that contains the CourseInscriptions for the student */

struct StudentCourse
{
	SchoolCourse *course;
	Group *lab_group;
	Group *theory_group;
};

class StudentSchedule
{
	public:
	typedef std::vector<struct StudentCourse *> course_list_t;
	
	void add_st_course(StudentCourse g);
	course_list_t::const_iterator st_courses_begin() { return p_courses.begin(); }
	course_list_t::const_iterator st_courses_end() { return p_courses.end(); }
	
	private:
	course_list_t p_courses;
};

#endif // STUDENTSCHEDULE_HPP
