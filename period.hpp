#ifndef PERIOD_HPP
#define PERIOD_HPP

#include <string>

struct Period
{
	public:
	Period() {}
	
	std::string room() { return p_room; }
	int period_no() { return p_period_no; }
	
	void set_room(std::string r) { p_room = r; }
	void set_period_no(int n) { p_period_no = n; }

	private:
	int p_period_no;
	std::string p_room;
};

#endif // PERIOD_HPP
