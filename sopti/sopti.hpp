#include <set>
#include <string>

#include "course.hpp"

class CourseSymOrder
{
	public:
	CourseSymOrder() {}
	bool operator()(const Course &a, const Course &b) {
		return a.symbol() < b.symbol();
	}
};

class Sopti
{
	public:

	typedef std::set<Course,CourseSymOrder> course_list_t;

	Sopti();
	void load_info_from_csv(std::string, std::string);
	bool course_exists(std::string);
	void course_add(Course);
	course_list_t::const_iterator courses_begin() { return p_courses.begin(); }
	course_list_t::const_iterator courses_end() { return p_courses.end(); }
	
	private:
	std::set<Course, CourseSymOrder> p_courses;
};
