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
#include <string>

#include "studentschedule.hpp"
#include "schoolschedule.hpp"

/* A constraint returns true if it is realized */

class Constraint
{
	public:
	Constraint(std::string) {}
	
	virtual bool operator()(StudentSchedule &) = 0;
	
	private:
};

class NoConflicts : public Constraint
{
	public:
	NoConflicts(std::string s) : Constraint(s) {
		char *endptr;
		p_max_conflict_periods = strtol(s.c_str(), &endptr, 10);
		if(*endptr != 0) {
			error("Invalid max conflict periods (%s)", s.c_str());
		}	
	}
	
	bool operator()(StudentSchedule &sched)
	{
		std::set<int> used_periods;
	
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		int c_hours=0;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					if(used_periods.find((*it2)->period_no()) != used_periods.end()) {
						if(++c_hours >= p_max_conflict_periods)
							return false;
					}
					used_periods.insert((*it2)->period_no());
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					if(used_periods.find((*it2)->period_no()) != used_periods.end()) {
						if(++c_hours > p_max_conflict_periods)
							return false;
					}
					used_periods.insert((*it2)->period_no());
				}
			}
		}
		return true;
	}
	
	private:
	int p_max_conflict_periods;
};



class NoEvening : public Constraint
{
	public:
	NoEvening(std::string s) : Constraint(s) {}
	
	bool operator()(StudentSchedule &sched)
	{
		const int last_allowed=1745;
		
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					if(poly_period_to_time((*it2)->period_no()) > last_allowed)
						return false;
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					if(poly_period_to_time((*it2)->period_no()) > last_allowed)
						return false;
				}
			}
		}
		return true;
	}
	
	private:
};

class NoClosed : public Constraint
{
	public:
	NoClosed(std::string s) : Constraint(s) {}
	
	bool operator()(StudentSchedule &sched)
	{
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				if((*it)->theory_group->closed()) {
					return false;
				}
			}
			if((*it)->lab_group) {
				if((*it)->lab_group->closed()) {
					return false;
				}
			}
		}
		return true;
	}
	
	private:
};

class NoPeriod : public Constraint
{
	public:
	NoPeriod(std::string s) : Constraint(s) { 
		char *endptr;
		p_period = strtol(s.c_str(), &endptr, 10);
		if(*endptr != 0) {
			error("Invalid parameter passed to NoPeriod (%s)", s.c_str());
		}
	}
	
	bool operator()(StudentSchedule &sched)
	{
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					if((*it2)->period_no() == p_period)
						return false;
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					if((*it2)->period_no() == p_period)
						return false;
				}
			}
		}
		return true;
	}
	
	private:
	long p_period;
};

