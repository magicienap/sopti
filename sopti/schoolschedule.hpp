#ifndef SCHOOLSCHEDULE_HPP
#define SCHOOLSCHEDULE_HPP

#include <map>
#include <vector>
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
	void course_add(SchoolCourse);
	course_list_t::const_iterator courses_begin() { return p_courses.begin(); }
	course_list_t::const_iterator courses_end() { return p_courses.end(); }
	
	private:
	// The courses, no particular order
	std::vector<SchoolCourse *> p_courses;
	// The courses, ordered by symbol
	std::map<std::string, SchoolCourse *> p_courses_order_sym;
};

#endif // SCHOOLSCHEDULE_HPP
