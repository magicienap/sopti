#include <string>
#include <vector>
#include <mysql/mysql.h>

#include "schoolschedule.hpp"
#include "globals.hpp"

class DBLoader
{
	public:
	DBLoader();
	~DBLoader();
	SchoolSchedule * get_ss_with_courses(std::vector<std::string> *);
	SchoolSchedule * get_ss_with_all(void);
	std::vector<std::string> get_course_list(void);
	void connect();
	
	private:
	MYSQL *pmysql;
	void read_courses_into(std::vector<std::string> *, SchoolSchedule *);
};
