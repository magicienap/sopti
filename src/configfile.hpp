#include <string>
#include <map>

extern std::map<std::string,std::string> config_vars;
void parse_config_file(std::string conffile_name);
void parse_semester_file(std::string semester_file_name);
