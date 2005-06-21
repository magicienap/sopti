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

#include "group.hpp"


/* ------------------------------------------------------------------

	Function: add_period
	Description: add a period to the group
	Parameters: p, the period to add
	Return value: none
	Notes: none

------------------------------------------------------------------ */

void Group::add_period(Period p)
{
	Period *tmp = new Period(p);
	p_periods.push_back(tmp);
	p_periods_by_no[p.period_no()] = tmp;
}


/* ------------------------------------------------------------------

	Function: has_period
	Description: returns wether a period is part of the group or not
		based on the period's number
	Parameters: p, the period's number
	Return value: boolean, true if the group is part of the course
	Notes: none

------------------------------------------------------------------ */

bool Group::has_period(int p)
{
	return p_periods_by_no.find(p) != p_periods_by_no.end();
}


/* ------------------------------------------------------------------

	Function: group
	Description: get a pointer to the Period object of a
		a certain period
	Parameters: p, the period's number
	Return value: the Period object that has the number p
	Notes: the returned pointer must not be deallocated

------------------------------------------------------------------ */

Period *Group::periods(int p)
{
	if(!has_period(p))
		return 0;

	return p_periods_by_no[p];
}
