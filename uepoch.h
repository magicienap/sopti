#include <sys/time.h>

inline long long uepoch(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long) tv.tv_sec * 1000000 + tv.tv_usec;
}
