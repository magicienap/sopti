#include <set>

#include "studentschedule.hpp"
#include "schoolschedule.hpp"

/* A constraint returns true if it is realized */

class Constraint
{
	public:
	Constraint() {}
	
	virtual bool operator()(StudentSchedule &) = 0;
	
	private:
};

class NoConflicts : public Constraint
{
	public:
	NoConflicts(SchoolSchedule *ss) { p_school_sched = ss; }
	
	bool operator()(StudentSchedule &sched)
	{
		std::set<int> used_periods;
	
		StudentSchedule::course_list_t::const_iterator it;
		Group::period_list_t::const_iterator it2;
		
		// Iterate across all courses in the schedule
		for(it=sched.st_courses_begin(); it!=sched.st_courses_end(); it++) {
			if((*it)->theory_group) {
				// Iterate across all periods of this course
				for(it2=(*it)->theory_group->periods_begin(); it2!=(*it)->theory_group->periods_end(); it2++) {
					if(used_periods.find((*it2)->period_no()) != used_periods.end()) {
						return false;
					}
					used_periods.insert((*it2)->period_no());
				}
			}
			if((*it)->lab_group) {
				for(it2=(*it)->lab_group->periods_begin(); it2!=(*it)->lab_group->periods_end(); it2++) {
					if(used_periods.find((*it2)->period_no()) != used_periods.end()) {
						return false;
					}
					used_periods.insert((*it2)->period_no());
				}
			}
		}
		return true;
	}
	
	private:
	SchoolSchedule *p_school_sched;
};
