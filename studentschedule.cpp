#include "studentschedule.hpp"

void StudentSchedule::add_st_course(StudentCourse g)
{
	StudentCourse *tmp;
	
	tmp = new StudentCourse(g);
	p_courses.push_back(tmp);
}
