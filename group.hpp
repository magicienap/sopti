#include <vector>

#include "period.hpp"

class Group
{
	public:
	Group(std::string n) { p_group_name = n; p_islab=false; }
	
	void set_lab(bool l) { p_islab = l; }
	void add_period(Period);
	
	std::string name() { return p_group_name; }
	bool lab() { return p_islab; }
	
	private:
	std::string p_group_name;
	std::vector<Period *> p_periods;
	bool p_islab;
};
