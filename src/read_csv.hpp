#ifndef READ_CSV_HPP
#define READ_CSV_HPP

#include <string>
#include <vector>
class SchoolSchedule;

#define COURSEFILE_FIELD_CYCLE		0
#define COURSEFILE_FIELD_SYMBOL		1
#define COURSEFILE_FIELD_GROUP		2
#define COURSEFILE_FIELD_CREDITS	3
#define COURSEFILE_FIELD_ROOM		6
#define COURSEFILE_FIELD_COURSELAB	7
#define COURSEFILE_FIELD_LABWEEK	9
#define COURSEFILE_FIELD_COURSETYPE	10
#define COURSEFILE_FIELD_TITLE		11
#define COURSEFILE_FIELD_DAY		13
#define COURSEFILE_FIELD_TIME		14

#define CLOSEDFILE_FIELD_SYMBOL		1
#define CLOSEDFILE_FIELD_GROUP		2
#define CLOSEDFILE_FIELD_COURSELAB	3

std::vector<std::string> split_string(std::string s, std::string sep);
int poly_make_period_no(std::string day_of_week, std::string time_str);
void load_courses_from_csv(SchoolSchedule *sopti, std::string periods_file);
void load_closed_from_csv(SchoolSchedule *sopti,  std::string closed_file);

#endif // READ_CSV_HPP
