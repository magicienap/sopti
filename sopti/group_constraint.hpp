/*  School Schedule Optimizer
 *  Copyright (C) 2005  Pierre-Marc Fournier <pmf@users.sf.net>
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
#include "group.hpp"

/* A constraint returns true if it is passed, false if it is failed */

class GroupConstraint
{
	public:
	GroupConstraint(std::string) {}
	
	virtual bool operator()(Group *, SchoolCourse *) = 0;
	
	virtual ~GroupConstraint() {}
	
	private:
};

class NoClosed : public GroupConstraint
{
	public:
	NoClosed(std::string s) : GroupConstraint(s) {}
	
	bool operator()(Group *group, SchoolCourse *)
	{
		if(group->closed())
			return false;

		return true;
	}
	
	private:
};

class NoPeriod : public GroupConstraint
{
	public:
	NoPeriod(std::string s) : GroupConstraint(s) { 
		char *endptr;
		p_period = strtol(s.c_str(), &endptr, 10);
		if(*endptr != 0) {
			error("Invalid parameter passed to NoPeriod (%s)", s.c_str());
		}
	}
	
	bool operator()(Group *group, SchoolCourse *)
	{
		Group::period_list_t::const_iterator it;
		
		for(it=group->periods_begin(); it!=group->periods_end(); it++) {
			if((*it)->period_no() == p_period)
				return false;
		}
		return true;
	}
	
	private:
	long p_period;
};

class ExplicitOpen : public GroupConstraint
{
	public:
	ExplicitOpen(std::string s) : GroupConstraint(s)
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
	
	bool operator()(Group *group, SchoolCourse *course)
	{
		std::string mangled;
		
		// Iterate across all courses in the schedule
		if(!group->lab() || (group->lab() && course->type() == COURSE_TYPE_THEORYLABSAME)) {
			mangled = "t_" + to_variable_name(course->symbol()) + "_" + to_variable_name(group->name());
		}
		else {
			mangled = "l_" + to_variable_name(course->symbol()) + "_" + to_variable_name(group->name());
		}
		
		if(p_open.find(mangled) == p_open.end()) {
			return false;
		}
		
		return true;
	}
	
	private:
	std::set<std::string> p_open;
};

class NotBetween : public GroupConstraint
{
	public:
	NotBetween(std::string s) : GroupConstraint(s)
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
	
	bool operator()(Group *group, SchoolCourse *)
	{
		Group::period_list_t::const_iterator it;
		int period;
		
		for(it=group->periods_begin(); it!=group->periods_end(); it++) {
			period = (*it)->period_no();
			if(period >= p_min && period <= p_max) {
				return false;
			}
		}
		return true;
	}
	
	private:
	int p_min;
	int p_max;
};
