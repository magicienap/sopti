#include <mysql/mysql.h>
#include "dbloader.hpp"
#include "make_message.h"
#include "globals.hpp"
#include "configfile.hpp"

using namespace std;

DBLoader::DBLoader()
{
	pmysql = 0;

}

DBLoader::~DBLoader()
{
	mysql_close(pmysql);
	pmysql=0;
}

/* DBLoader::connect() can be called anytime to ensure
 a database connection exists */

void DBLoader::connect(void)
{
	int result;
	MYSQL *res_mysql;

	if(pmysql != 0) {
		/* If a connection was made in the past, just make
                   sure it's still alive */
		result = mysql_ping(pmysql);
		if(result != 0) {
			error("Unable to connect to database");
		}
		/* Else everything went fine */
	}
	
	pmysql = new MYSQL;
	res_mysql = mysql_init(pmysql);
	if(res_mysql == 0) {
		// insufficient memory
		error("Insufficient memory: %s", mysql_error(pmysql));
	}
	
	res_mysql = mysql_real_connect(pmysql, config_vars["db.host"].c_str(), config_vars["db.username"].c_str(), config_vars["db.password"].c_str(), config_vars["db.schema"].c_str(), 0, 0, 0);
	if(res_mysql == 0) {
		// connection error
		error("mySQL connection error: %s", mysql_error(pmysql));
	}

}

vector<string> DBLoader::get_course_list(void)
{
	char *query;
	char *param1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int result;
	
	connect();
	
	// Create the schoolschedule that will be returned
	vector<string> retval;
	
	vector<string>::const_iterator it;
	
	// TODO: do something for cases where several semesters exist
	query = make_message("SELECT courses.symbol FROM courses_semester LEFT JOIN courses ON courses.unique=courses_semester.course LEFT JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='%s' ORDER BY courses.symbol", config_vars["default_semester"].c_str());
	// Do query
	result = mysql_query(pmysql, query);
	if(result) {
		error("error in mysql_query: %s", mysql_error(pmysql));
	}
	free(query);
	// Extract data
	res = mysql_store_result(pmysql);
	if(res == 0) {
		error("unable to store result");
	}
	
	while(row = mysql_fetch_row(res)) {
		retval.push_back(row[0]);
	}
	
	mysql_free_result(res);
	return retval;
}

void DBLoader::read_courses_into(vector<string> *c, SchoolSchedule *sched)
{
	char *query;
	char *param1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int result;
	vector<string>::const_iterator it;
	
	// For each course we want to read
	for(it=c->begin(); it!=c->end(); it++) {
		// Prepare request
		param1 = new char[it->size()*2+1];
		mysql_real_escape_string(pmysql, param1, it->c_str(), it->size());
		// TODO: do something for cases where several semesters exist
		query = make_message("SELECT courses.symbol,courses.title,courses_semester.course_type,courses_semester.unique FROM courses INNER JOIN courses_semester ON courses.unique=courses_semester.course WHERE courses.symbol='%s' AND semesters.code='%s'", param1, config_vars["default_semester"].c_str());
		delete [] param1;
		// Do query
		result = mysql_query(pmysql, query);
		if(result) {
			error("error in mysql_query: %s", mysql_error(pmysql));
		}
		free(query);
		// Extract data
		res = mysql_store_result(pmysql);
		if(res == 0) {
			error("unable to store result");
		}
		
		row = mysql_fetch_row(res);
		// assert only one row
		// Make the course object
		SchoolCourse *newcourse = new SchoolCourse(row[0]);
		newcourse->set_title(row[1]);
		if(!strcmp(row[2], "T")) {
			newcourse->set_type(COURSE_TYPE_THEORYONLY);
		}
		else if(!strcmp(row[2], "L")) {
			newcourse->set_type(COURSE_TYPE_LABONLY);
		}
		else if(!strcmp(row[2], "TL")) {
			newcourse->set_type(COURSE_TYPE_THEORYLABSAME);
		}
		else if(!strcmp(row[2], "TLS")) {
			newcourse->set_type(COURSE_TYPE_THEORYLABIND);
		}
		else {
			error("invalid course type");
		}

		int course_semester_unique = atoi(row[3]);
		
		mysql_free_result(res);
		
		sched->add_course(newcourse);
		
		// Get groups and save
		query = make_message("SELECT groups.unique,groups.name,groups.theory_or_lab,groups.teacher,groups.places_room,groups.places_group,groups.places_taken FROM groups WHERE groups.course_semester='%u'", course_semester_unique);
		mysql_query(pmysql, query);
		free(query);
		
		res = mysql_store_result(pmysql);
		if(res == 0) {
			error("unable to store result");
		}
		
		vector< pair<int, Group *> > group_uniques;
		
		while(row = mysql_fetch_row(res)) {
			Group *newgroup = new Group(row[1]);
			bool lab;

			newgroup->set_closed(false);
			
			if(!strcmp(row[2], "C")) {
				lab = false;
			}
			else if(!strcmp(row[2], "L")) {
				lab = true;
			}
			else {
				error("invalid course/lab");
			}
			
			newgroup->set_lab(lab);
			
			group_uniques.push_back(pair<int, Group *>(atoi(row[0]), newgroup));
			newcourse->add_group(newgroup, lab);
		}
		
		mysql_free_result(res);
		
		// Get periods and save
		vector< pair<int, Group *> >::const_iterator it2;
		for(it2=group_uniques.begin(); it2!=group_uniques.end(); it2++) {
			query = make_message("SELECT periods.period_code,periods.weekday,periods.time,periods.room,periods.week FROM periods WHERE periods.group='%u'", it2->first);
			mysql_query(pmysql, query);
			free(query);
			
			res = mysql_store_result(pmysql);
			if(res == 0) {
				error("unable to store result");
			}
			
			while(row = mysql_fetch_row(res)) {
				Period *newperiod = new Period();
				newperiod->set_period_no(poly_make_period_no(row[1], row[2]));
				
				/* Set the room */
				newperiod->set_room(row[3]);

				/* Set the week */
				if(!strcmp(row[4], "A")) {
					newperiod->set_week(0);
				}
				else if(!strcmp(row[4], "B1")) {
					newperiod->set_week(1);
				}
				else if(!strcmp(row[4], "B2")) {
					newperiod->set_week(2);
				}
				else {
					error("Period has unknown week in database");
				}
				
				it2->second->add_period(*newperiod);
				delete newperiod;
			}
		}
		
		mysql_free_result(res);
	}
}

SchoolSchedule * DBLoader::get_ss_with_courses(vector<string> *c)
{
	char *query;
	char *param1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int result;
	
	connect();
	
	// Create the schoolschedule that will be returned
	SchoolSchedule *retval = new SchoolSchedule();
	
	vector<string>::const_iterator it;
	
	// For each course we want to read
	for(it=c->begin(); it!=c->end(); it++) {
		// Prepare request
		param1 = new char[it->size()*2+1];
		mysql_real_escape_string(pmysql, param1, it->c_str(), it->size());
		// TODO: do something for cases where several semesters exist
		query = make_message("SELECT courses.symbol,courses.title,courses_semester.course_type,courses_semester.unique FROM courses INNER JOIN courses_semester ON courses.unique=courses_semester.course INNER JOIN semesters ON semesters.unique=courses_semester.semester WHERE semesters.code='%s' AND courses.symbol='%s'", config_vars["default_semester"].c_str(), param1);
		delete [] param1;
		// Do query
		result = mysql_query(pmysql, query);
		if(result) {
			error("error in mysql_query: %s", mysql_error(pmysql));
		}
		free(query);
		// Extract data
		res = mysql_store_result(pmysql);
		if(res == 0) {
			error("unable to store result");
		}
		
		row = mysql_fetch_row(res);
		if(row == 0) {
			error("unknown course: %s", it->c_str());
		}
		// assert only one row
		// Make the course object
		SchoolCourse *newcourse = new SchoolCourse(row[0]);
		newcourse->set_title(row[1]);
		if(!strcmp(row[2], "T")) {
			newcourse->set_type(COURSE_TYPE_THEORYONLY);
		}
		else if(!strcmp(row[2], "L")) {
			newcourse->set_type(COURSE_TYPE_LABONLY);
		}
		else if(!strcmp(row[2], "TL")) {
			newcourse->set_type(COURSE_TYPE_THEORYLABSAME);
		}
		else if(!strcmp(row[2], "TLS")) {
			newcourse->set_type(COURSE_TYPE_THEORYLABIND);
		}
		else {
			error("invalid course type");
		}

		int course_semester_unique = atoi(row[3]);
		
		mysql_free_result(res);
		
		retval->add_course(newcourse);
		
		// Get groups and save
		query = make_message("SELECT groups.unique,groups.name,groups.theory_or_lab,groups.teacher,groups.places_room,groups.places_group,groups.places_taken FROM groups WHERE groups.course_semester='%u'", course_semester_unique);
		mysql_query(pmysql, query);
		free(query);
		
		res = mysql_store_result(pmysql);
		if(res == 0) {
			error("unable to store result");
		}
		
		vector< pair<int, Group *> > group_uniques;
		
		while(row = mysql_fetch_row(res)) {
			Group *newgroup = new Group(row[1]);
			bool lab;

			newgroup->set_closed(false);
			
			if(!strcmp(row[2], "C")) {
				lab = false;
			}
			else if(!strcmp(row[2], "L")) {
				lab = true;
			}
			else {
				error("invalid course/lab");
			}
			
			newgroup->set_lab(lab);
			
			group_uniques.push_back(pair<int, Group *>(atoi(row[0]), newgroup));
			newcourse->add_group(newgroup, lab);
		}
		
		mysql_free_result(res);
		
		// Get periods and save
		vector< pair<int, Group *> >::const_iterator it2;
		for(it2=group_uniques.begin(); it2!=group_uniques.end(); it2++) {
			query = make_message("SELECT periods.period_code,periods.weekday,periods.time,periods.room,periods.week FROM periods WHERE periods.group='%u'", it2->first);
			mysql_query(pmysql, query);
			free(query);
			
			res = mysql_store_result(pmysql);
			if(res == 0) {
				error("unable to store result");
			}
			
			while(row = mysql_fetch_row(res)) {
				Period *newperiod = new Period();
				newperiod->set_period_no(poly_make_period_no(row[1], row[2]));
				
				/* Set the room */
				newperiod->set_room(row[3]);

				/* Set the week */
				if(!strcmp(row[4], "A")) {
					newperiod->set_week(0);
				}
				else if(!strcmp(row[4], "B1")) {
					newperiod->set_week(1);
				}
				else if(!strcmp(row[4], "B2")) {
					newperiod->set_week(2);
				}
				else {
					error("Period has unknown week in database");
				}
				
				it2->second->add_period(*newperiod);
				delete newperiod;
			}
		}
		
		mysql_free_result(res);
	}
	return retval;
}

/*
SchoolSchedule * DBLoader::get_ss_with_all(void)
{
	char *query;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int result;

	SchoolSchedule *retval = new SchoolSchedule();
	vector<string> all_courses;
	
	connect();
	
	query = make_message("SELECT courses.symbol from courses");
	result = mysql_query(pmysql, query);
	if(result) {
		error("error in mysql_query: %s", mysql_error(pmysql));
	}
	free(query);
	
	res = mysql_store_result(pmysql);
	if(res == 0) {
		error("unable to store result");
	}
		
	while(row = mysql_fetch_row(res)) {
		all_courses.push_back(row[0]);
	}
	
	mysql_free_result(res);
	
	read_courses_into(&all_courses, retval);
	
	return retval;
}
*/

