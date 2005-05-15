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

bool SchoolCourse::group_exists(string g, int islab)
{
	if(islab) {
		return p_lab_groups.find(g) != p_lab_groups.end();
	}
	else {
		return p_theory_groups.find(g) != p_theory_groups.end();
	}
}

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
