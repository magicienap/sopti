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

#ifndef PERIOD_HPP
#define PERIOD_HPP

#include <string>

struct Period
{
	public:
	Period() {}
	
	std::string room() const { return p_room; }
	int period_no() const { return p_period_no; }
	int week() const { return p_week; }
	
	void set_room(std::string r) { p_room = r; }
	void set_period_no(int n) { p_period_no = n; }
	void set_week(int w) { p_week = w; }

	private:
	int p_period_no;
	std::string p_room;
	int p_week;
};

#endif // PERIOD_HPP
