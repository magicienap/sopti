#ifndef COURSE_HPP
#define COURSE_HPP

#include <string>
#include <set>

#include "group.hpp"

class Course
{
	public:
	Course(std::string symbol) { p_symbol = symbol; }
	std::string symbol() const { return p_symbol; }
	std::string title() const { return p_title; }
	
	void set_title(std::string s) { p_title = s; }
	
	private:
	std::string p_symbol;
	std::string p_title;
	std::set<Group> p_theory_groups;
	std::set<Group> p_lab_groups;
};

#endif // COURSE_HPP
