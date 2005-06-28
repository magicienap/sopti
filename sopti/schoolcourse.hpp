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


#ifndef SCHOOLCOURSE_HPP
#define SCHOOLCOURSE_HPP

#include <string>
#include <map>
#include <vector>

#include "group.hpp"

#define COURSE_TYPE_THEORYONLY		1
#define COURSE_TYPE_LABONLY		2
#define COURSE_TYPE_THEORYLABIND	3
#define COURSE_TYPE_THEORYLABSAME	4

class SchoolCourse
{
	public:
	typedef std::vector<Group *> group_list_t;
	
	SchoolCourse(std::string symbol) { p_symbol = symbol; }
	~SchoolCourse();
	
	bool group_exists(std::string, int islab);
	Group *group(std::string, bool);
	
	// Data access
	std::string symbol() const { return p_symbol; }
	std::string title() const { return p_title; }
	int type() const { return p_type; }
	group_list_t::const_iterator groups_begin() const { return p_groups.begin(); }
	group_list_t::const_iterator groups_end() const { return p_groups.end(); }
	
	// Data set
	void set_title(std::string s) { p_title = s; }
	void set_type(int t) { p_type = t; }
	
	// Add groups
	void add_group(Group, bool); // deprecated
	void add_group(Group *, bool);
	
	private:
	std::string p_symbol;
	std::string p_title;
	int p_type;
	
	// All groups in a basic container
	group_list_t p_groups;
	
	std::map<std::string, Group *> p_theory_groups;
	std::map<std::string, Group *> p_lab_groups;
};

#endif // SCHOOLCOURSE_HPP
