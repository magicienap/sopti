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

#include <set>
 
#include "studentschedule.hpp"

class Objective
{
	public:
	virtual float operator()(StudentSchedule *)=0;
	
	private:
};

class MinHoles : public Objective
{
	public:
	float operator()(StudentSchedule *s)
	{
		int hours_week[] = { 830, 930, 1030, 1130, 1245, 1345, 1445, 1545, 1645, -1 };
		unsigned int i,j;
		std::set<int> occupied_periods;
		int holes=0;
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		for(it=s->st_courses_begin(); it!=s->st_courses_end(); it++) {
			// If has theorical class
			if((*it)->theory_group) {
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					occupied_periods.insert((*it2)->period_no());
					debug("inserted %d", (*it2)->period_no());
				}
			}
			// If has lab class
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					occupied_periods.insert((*it2)->period_no());
					debug("inserted %d", (*it2)->period_no());
				}
			}
		}
	
		// For each day monday-friday
		for(i=0; i<5; i++) {
			int accum=0; // Temporary counter of free periods
			bool day_started=false;
			
			for(j=0; hours_week[j]!=-1; j++) {
				
				if(occupied_periods.find(10000*(i+1)+hours_week[j]) == occupied_periods.end()) {
					// If free period
					if(day_started) {
						accum++;
					}
				}
				else {
					// In period
					day_started=true;
					holes+=accum;
					accum=0;
				}
			}
		}
		return holes;
	}
};

class MaxMorningSleep : public Objective
{
	public:
	float operator()(StudentSchedule *s)
	{
		int hours_sleep[] = { 830, 930, 1030, 1130, -1 };
		unsigned int i,j;
		std::set<int> occupied_periods;
		int add_sleep=0;
		const int max_sleep=20;
		
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		for(it=s->st_courses_begin(); it!=s->st_courses_end(); it++) {
			// If has theorical class
			if((*it)->theory_group) {
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					occupied_periods.insert((*it2)->period_no());
					debug("inserted %d", (*it2)->period_no());
				}
			}
			// If has lab class
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					occupied_periods.insert((*it2)->period_no());
					debug("inserted %d", (*it2)->period_no());
				}
			}
		}
	
		// For each day monday-friday
		for(i=0; i<5; i++) {
			for(j=0; hours_sleep[j]!=-1; j++) {
				
				if(occupied_periods.find(10000*(i+1)+hours_sleep[j]) == occupied_periods.end()) {
					// If free period
					add_sleep++;
				}
				else {
					// In period
					break;

				}
			}
		}
		return max_sleep-add_sleep;
	}
};

class MaxFreeDays : public Objective
{
	public:
	float operator()(StudentSchedule *s)
	{
		std::set<int> occupied_days;
		
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		for(it=s->st_courses_begin(); it!=s->st_courses_end(); it++) {
			// If has theorical class
			if((*it)->theory_group) {
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					occupied_days.insert((*it2)->period_no()/10000);
				}
			}
			// If has lab class
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					occupied_days.insert((*it2)->period_no()/10000);
				}
			}
		}
		
		return occupied_days.size();
	}
};

class NullObjective : public Objective
{
	public:
	float operator()(StudentSchedule *)
	{
		return 0.;
	}
};
