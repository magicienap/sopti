#include <map>
#include "stdsched.h"

#define STDSCHED_NUMDAYS		7
#define STDSCHED_NUMDAYTEMPLATES	2
#define STDSCHED_MAXPERIODS		14

int stdsched_day2template[STDSCHED_NUMDAYS] = 
	{ 0, 0, 0, 0, 0, 1, 1 };
	
char stdsched_day2name[STDSCHED_NUMDAYS][10] = 
	{ "Lundi",
	  "Mardi",
	  "Mercredi",
	  "Jeudi",
	  "Vendredi",
	  "Samedi",
	  "Dimanche" };
	
struct stdsched_daytemplate stdsched_daytemplates[STDSCHED_NUMDAYTEMPLATES][STDSCHED_MAXPERIODS] = 
	{ { {  830,  920 }, 
	    {  930, 1020 }, 
	    { 1030, 1120 }, 
	    { 1130, 1220 }, 
	    { 1245, 1335 }, 
	    { 1345, 1435 }, 
	    { 1445, 1535 },
	    { 1545, 1635 },
	    { 1645, 1735 },
	    { 1800, 1900 },
	    { 1900, 2000 },
	    { 2000, 2100 },
	    { 2100, 2200 },
	    {   -1,   -1 } },
	    
	  { {  800,  900 },
	    {  900, 1000 },
	    { 1000, 1100 },
	    { 1100, 1200 },
	    { 1200, 1300 },
	    { 1300, 1400 },
	    { 1400, 1500 },
	    { 1500, 1600 },
	    { 1600, 1700 },
	    {   -1,   -1 } } };

// period_id = stdsched_hour2id[daytemplate_no][hour];
std::map<int, int> stdsched_hour2id[STDSCHED_NUMDAYTEMPLATES];

void stdsched_init()
{
	int i,j;
	for(i=0; i<STDSCHED_NUMDAYTEMPLATES; i++) {
		for(j=0; stdsched_daytemplates[i][j].start != -1; j++) {
			stdsched_hour2id[i][stdsched_daytemplates[i][j].start] = j;
		}
	}
}
