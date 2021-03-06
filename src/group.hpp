#ifndef GROUP_HPP
#define GROUP_HPP

#include <vector>
#include <map>

#include "period.hpp"

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

class Group
{
	public:
	typedef std::vector<Period *> period_list_t;
	Group(std::string n) { p_group_name = n; p_islab=false; p_closed=false; }
	~Group();
	
	bool has_period(int);
	Period *periods(int);
	
	void add_period(Period);
	void set_lab(bool l) { p_islab = l; }
	void set_closed(bool c) { p_closed = c; }
	
	std::string name() { return p_group_name; }
	bool lab() { return p_islab; }
	bool closed() { return p_closed; }
	
	period_list_t::const_iterator periods_begin() { return p_periods.begin(); }
	period_list_t::const_iterator periods_end() { return p_periods.end(); }
	
	private:
	std::string p_group_name;
	period_list_t p_periods;
	std::map<int, Period *> p_periods_by_no;
	bool p_islab;
	bool p_closed;
};

#endif // GROUP_HPP
