#include "schoolcourse.hpp"

using namespace std;

void SchoolCourse::add_group(Group g, bool islab)
{
	Group *tmp = new Group(g);

	p_groups.push_back(tmp);
	
	if(islab) {
		p_lab_groups[g.name()] = tmp;
	}
	else {
		p_theory_groups[g.name()] = tmp;
	}
}

bool SchoolCourse::group_exists(string g, int islab)
{
	if(islab) {
		return p_lab_groups.find(g) != p_lab_groups.end();
	}
	else {
		return p_theory_groups.find(g) != p_theory_groups.end();
	}
}

Group *SchoolCourse::group(std::string n, bool islab)
{
	if(!group_exists(n, islab))
		return 0;
		
	if(islab) {
		return p_lab_groups[n];
	}
	else {
		return p_theory_groups[n];
	}
}
