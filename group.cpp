#include "group.hpp"

void Group::add_period(Period p)
{
	Period *tmp = new Period(p);
	p_periods.push_back(tmp);
}
