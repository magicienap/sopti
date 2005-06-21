#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


/* ------------------------------------------------------------------

	Function: make_message
	Description: "allocate a sufficiently large string and print into
		it"
	Parameters: they work the same way as printf(3)
	Return value: a pointer to the string that was printed into
	Notes: comes from the printf(3) man page

------------------------------------------------------------------ */

char *make_message(const char *fmt, ...)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p;
	va_list ap;
	if ((p = malloc(size)) == NULL)
		return NULL;
	while (1) {
		/* Try to print in the allocated space. */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		/* Else try again with more space. */
		if (n > -1)	/* glibc 2.1 */
			size = n + 1;	/* precisely what is needed */
		else		/* glibc 2.0 */
			size *= 2;	/* twice the old size */
		if ((p = realloc(p, size)) == NULL)
			return NULL;
	}
}
