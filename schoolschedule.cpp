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

SchoolSchedule::SchoolSchedule()
{
}

bool SchoolSchedule::course_exists(string c)
{
	return p_courses_order_sym.find(c) != p_courses_order_sym.end();
}

void SchoolSchedule::course_add(SchoolCourse c)
{
	SchoolCourse *tmp = new SchoolCourse(c);
	
	// Add to the structures
	p_courses.push_back(tmp);
	p_courses_order_sym[tmp->symbol()] = tmp;
}

SchoolCourse *SchoolSchedule::course(std::string c)
{
	if(!course_exists(c))
		return 0;
		
	return p_courses_order_sym[c];
}
