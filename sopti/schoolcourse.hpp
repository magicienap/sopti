#ifndef SCHOOLCOURSE_HPP
#define SCHOOLCOURSE_HPP

#include <string>
#include <map>
#include <vector>

#include "group.hpp"

class SchoolCourse
{
	public:
	typedef std::vector<Group *> group_list_t;
	
	SchoolCourse(std::string symbol) { p_symbol = symbol; }
	
	bool group_exists(std::string, int islab);
	Group *group(std::string, bool);
	
	// Data access
	std::string symbol() const { return p_symbol; }
	std::string title() const { return p_title; }
	group_list_t::const_iterator groups_begin() const { return p_groups.begin(); }
	group_list_t::const_iterator groups_end() const { return p_groups.end(); }
	
	// Data set
	void set_title(std::string s) { p_title = s; }
	
	// Add groups
	void add_group(Group, bool);
	
	private:
	std::string p_symbol;
	std::string p_title;
	
	// All groups in a basic container
	std::vector<Group *> p_groups;
	
	std::map<std::string, Group *> p_theory_groups;
	std::map<std::string, Group *> p_lab_groups;
};

#endif // SCHOOLCOURSE_HPP
