#include <map>

#define STDSCHED_NUMDAYS		7
#define STDSCHED_NUMDAYTEMPLATES	2
#define STDSCHED_MAXPERIODS		14

struct stdsched_daytemplate {
	int start;
	int end;
};

extern int stdsched_day2template[STDSCHED_NUMDAYS];
extern char stdsched_day2name[STDSCHED_NUMDAYS][10];
extern struct stdsched_daytemplate stdsched_daytemplates[STDSCHED_NUMDAYTEMPLATES][STDSCHED_MAXPERIODS];
extern std::map<int, int> stdsched_hour2id[STDSCHED_NUMDAYTEMPLATES];

void stdsched_init();
