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
	std::vector<struct StudentCourse *> p_courses;
};

#endif // STUDENTSCHEDULE_HPP
