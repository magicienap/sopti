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

/* A constraint returns true if it is passed, false if it is failed */

class Constraint
{
	public:
	Constraint(std::string) {}
	
	virtual bool operator()(StudentSchedule &) = 0;
	
	virtual ~Constraint() {}
	
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
		std::map<int,int> used_periods;
	
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		std::map<int,int>::iterator it3;
		int c_hours=0;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					int week_mask;
					if((*it2)->week() == 0)
						week_mask = ~0;
					else
						week_mask = (1 << ((*it2)->week()-1));
						
					it3 = used_periods.find((*it2)->period_no());
					if(it3 != used_periods.end()) {
						if(it3->second & week_mask) {
							//debug("conflict between %x and %x<br>", it3->second, week_mask);
							if(++c_hours > p_max_conflict_periods)
								return false;
						}
						//debug("no conflict between %x and %x<br>", it3->second, week_mask);
						it3->second |= week_mask;
					}
					else {
						used_periods[(*it2)->period_no()] = week_mask;
					}
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					int week_mask;
					if((*it2)->week() == 0)
						week_mask = ~0;
					else
						week_mask = (1 << ((*it2)->week()-1));
						
					it3 = used_periods.find((*it2)->period_no());
					if(it3 != used_periods.end()) {
						if(it3->second & week_mask) {
							//debug("conflict between %x and %x<br>", it3->second, week_mask);
							if(++c_hours > p_max_conflict_periods)
								return false;
						}
						//debug("no conflict between %x and %x<br>", it3->second, week_mask);
						it3->second |= week_mask;
					}
					else {
						used_periods[(*it2)->period_no()] = week_mask;
					}
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

class ExplicitOpen : public Constraint
{
	public:
	ExplicitOpen(std::string s) : Constraint(s)
	{
		unsigned int start=0;
		unsigned int len;
		
		for(;;) {
			start = s.find_first_not_of(" ", start);
			if(start == std::string::npos)
				break;
			len = s.find_first_of(" ", start);
			if(len == std::string::npos)
				len = s.size();
			len -= start;
				
			p_open.insert(std::string(s, start, len));
			
			start += len;
			if(start >= s.size())
				break;
		}
	}
	
	bool operator()(StudentSchedule &sched)
	{
		std::string mangled;
		
		// Iterate across all courses in the schedule
		StudentSchedule::course_list_t::const_iterator it;
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				mangled = "t_" + to_variable_name((*it)->course->symbol()) + "_" + to_variable_name((*it)->theory_group->name());
				if(p_open.find(mangled) == p_open.end()) {
					return false;
				}
			}
			if((*it)->lab_group && (*it)->course->type() != COURSE_TYPE_THEORYLABSAME) {
				mangled = "l_" + to_variable_name((*it)->course->symbol()) + "_" + to_variable_name((*it)->lab_group->name());
				if(p_open.find(mangled) == p_open.end()) {
					return false;
				}
			}
		}
		
		return true;
	}
	
	private:
	std::set<std::string> p_open;
};

class NotBetween : public Constraint
{
	public:
	NotBetween(std::string s) : Constraint(s)
	{
		p_min = -1;
		p_max = -1;
		
		unsigned int start=0,len;
	
		// Min
		start = s.find_first_not_of(" ", start);
		if(start == std::string::npos)
			goto error;
		len = s.find_first_of(" ", start);
		if(len == std::string::npos)
			len = s.size();
		len -= start;
	
		p_min = atoi(std::string(s, start, len).c_str());

		start += len;
		if(start >= s.size())
			goto error;
		
		// Max	
		start = s.find_first_not_of(" ", start);
		if(start == std::string::npos)
			goto error;
		len = s.find_first_of(" ", start);
		if(len == std::string::npos)
			len = s.size();
		len -= start;
	
		p_max = atoi(std::string(s, start, len).c_str());
		
		
		return;
		
		error:
		error("invalid parameter to constraint notbetween");
	}
	
	bool operator()(StudentSchedule &sched)
	{
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			int period;
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					period = (*it2)->period_no();
					if(period >= p_min && period <= p_max) {
						return false;
					}
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					period = (*it2)->period_no();
					if(period >= p_min && period <= p_max) {
						return false;
					}
				}
			}
		}
		return true;
	}
	
	private:
	int p_min;
	int p_max;
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

