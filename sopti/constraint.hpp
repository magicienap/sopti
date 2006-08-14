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
#include "error.hpp"


/* ------------------------------------------------------------------

	Class: Constraint
	Description: returns true if the constraint on a schedule is
		respected, false otherwise
	Constructor parameters: a string whose meaning depends on the
		specific Constraint derived class
	Parameters: StudentSchedule objet to evaluate
	Return value: boolean, true if the constraint on a schedule is
		respected
	Notes: this class defines a functor, this is also and abstract 
		class,	the operator() has to be implemented in derived 
		constraint classes

------------------------------------------------------------------ */

class Constraint
{
	public:
	Constraint(std::string) {}
	virtual bool operator()(StudentSchedule &) = 0;
	
	virtual ~Constraint() {}
	
	private:
};


/* ------------------------------------------------------------------

	Class: NoConflicts
	Description: returns true if a StudentSchedule has less than a
		certain number of overlapping periods
	Constructor parameters: s, a string containing the maximum number
		of conflicts
	Parameters: sched, the StudentSchedule objet to evaluate for 
		conflicts
	Return value: boolean, true if the number of overlapping periods
		in the schedule is smaller than the parameter s
	Notes: this class defines a functor

------------------------------------------------------------------ */

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

class MinimalCourseCount : public Constraint
{
        public:
        MinimalCourseCount(std::string s) : Constraint(s) {
                char *endptr;
                p_count = strtol(s.c_str(), &endptr, 10);
                if(*endptr != 0) {
                        error("Invalid parameter passed to MinimalCourseCount (%s)", s.c_str());
                }
        }

        bool operator()(StudentSchedule &sched)
        {
                if(sched.course_count() >= p_count) {
                        return true;
                }
                return false;
        }

        private:
        long p_count;
};

