#ifndef GROUP_HPP
#define GROUP_HPP

#include <vector>

#include "period.hpp"

class Group
{
	public:
	typedef std::vector<Period *> period_list_t;
	Group(std::string n) { p_group_name = n; p_islab=false; }
	
	void set_lab(bool l) { p_islab = l; }
	void add_period(Period);
	
	std::string name() { return p_group_name; }
	bool lab() { return p_islab; }
	
	period_list_t::const_iterator periods_begin() { return p_periods.begin(); }
	period_list_t::const_iterator periods_end() { return p_periods.end(); }
	
	private:
	std::string p_group_name;
	std::vector<Period *> p_periods;
	bool p_islab;
};

#endif // GROUP_HPP
