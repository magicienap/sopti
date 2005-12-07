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


#include "schoolcourse.hpp"

using namespace std;

SchoolCourse::~SchoolCourse()
{
	group_list_t::const_iterator it;
	for(it = groups_begin(); it != groups_end(); it++) {
		delete *it;
	}
}

/* ------------------------------------------------------------------

	Function: add_group
	Description: add a group to the course
	Parameters: - g, the group to add
		- islab, true if the group is a lab group
	Return value: none
	Notes: the function add_group that takes a pointer as the first
		parameter should be prefered over this one

------------------------------------------------------------------ */

void SchoolCourse::add_group(Group g, bool islab)
{
	Group *tmp = new Group(g);

	p_groups.push_back(tmp);
	
	if(islab) {
		p_lab_groups[g.name()] = tmp;
	}
	else {
		p_theory_groups[g.name()] = tmp;
	}
}


/* ------------------------------------------------------------------

	Function: add_group
	Description: add a group to the course
	Parameters: - g, a pointer to the group to add
		- islab, true if the group is a lab group
	Return value: none
	Notes: g must not be deallocated afterwards

------------------------------------------------------------------ */

void SchoolCourse::add_group(Group *g, bool islab)
{
	p_groups.push_back(g);
	
	if(islab) {
		p_lab_groups[g->name()] = g;
	}
	else {
		p_theory_groups[g->name()] = g;
	}
}


/* ------------------------------------------------------------------

	Function: group_exists
	Description: returns wether a group is part of the course or not
		based on the group's symbol
	Parameters: - g, the group's symbol
		- islab, true if the group is a lab group
	Return value: boolean, true if the group is part of the course
	Notes: none

------------------------------------------------------------------ */

bool SchoolCourse::group_exists(string g, int islab)
{
	if(islab) {
		return p_lab_groups.find(g) != p_lab_groups.end();
	}
	else {
		return p_theory_groups.find(g) != p_theory_groups.end();
	}
}


/* ------------------------------------------------------------------

	Function: group
	Description: get a pointer to the Group object of a
		a certain group
	Parameters: - n, the desired group's symbol
		- islab, true if the group is a lab group
	Return value: the Group object that has the symbol n and lab
		state islab
	Notes: the returned pointer must not be deallocated

------------------------------------------------------------------ */

Group *SchoolCourse::group(std::string n, bool islab)
{
	if(!group_exists(n, islab))
		return 0;
		
	if(islab) {
		return p_lab_groups[n];
	}
	else {
		return p_theory_groups[n];
	}
}
