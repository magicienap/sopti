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
