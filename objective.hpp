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


/* ------------------------------------------------------------------

	Class: Objective
	Description: returns the score of a student schedule according to
		a certain objective, the lower the score, the better the
		schedule
	Constructor parameters: none
	Parameters: StudentSchedule object to evaluate
	Return value: float, the lower the number, the better the 
		schedule
	Notes: this class defines a functor, this is also and abstract 
		class,	the operator() has to be implemented in derived 
		objective classes

------------------------------------------------------------------ */

class Objective
{
	public:
	virtual float operator()(StudentSchedule *)=0;
	
	private:
};


/* ------------------------------------------------------------------

	Class: MinHoles
	Description: objective aimed at having the least possible number
		of free hours between occupied periods in a day, therefore 
		lessening the time "lost" at school between classes
	Constructor parameters: none
	Parameters: s, a pointer to the StudentSchedule object to 
		evaluate
	Return value: float, the number of periods that are holes
	Notes: this class defines a functor, holes are only counted
		between periods that are during "standard" hours

------------------------------------------------------------------ */

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
		
		// The program first lists all occupied periods
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
		
		// Then the program searches for holes between the occupied periods on a day by day basis
		// For each day monday-friday:
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


/* ------------------------------------------------------------------

	Class: MaxMorningSleep
	Description: objective aimed at having the most possible number
		of free hours in the morning
	Constructor parameters: none
	Parameters: s, a pointer to the StudentSchedule object to 
		evaluate
	Return value: float, 20 - the total number of class hours in the 
		morning
	Notes: this class defines a functor

------------------------------------------------------------------ */

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


/* ------------------------------------------------------------------

	Class: MaxFreeDays
	Description: objective aimed at having the most possible number
		of free days
	Constructor parameters: none
	Parameters: s, a pointer to the StudentSchedule object to 
		evaluate
	Return value: float, the number of occupied days
	Notes: this class defines a functor

------------------------------------------------------------------ */

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

class MaxCourses : public Objective
{
	public:
	float operator()(StudentSchedule *s)
	{
		return 1./static_cast<float>(s->course_count());
	}
};

/* ------------------------------------------------------------------

	Class: NullObjective
	Description: bogus objective that always returns 0 and can be
		used when no other objective is selected
	Constructor parameters: none
	Parameters: StudentSchedule object to evaluate (ignored)
	Return value: float, 0
	Notes: this class defines a functor

------------------------------------------------------------------ */

class NullObjective : public Objective
{
	public:
	float operator()(StudentSchedule *)
	{
		return 0.;
	}
};
