#include <string>

#include "sopti.hpp"

using namespace std;

Sopti::Sopti() //: p_courses(CourseSymOrder())
{
}

bool Sopti::course_exists(string c)
{
	return p_courses.find(c) != p_courses.end();
}

void Sopti::course_add(Course c)
{
	p_courses.insert(c);
}
