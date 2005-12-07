/*  School Schedule Optimizer
 *  Copyright (C) 2004  Pierre-Marc Fournier <pmf@users.sf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "error.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>

#include "globals.hpp"

using namespace std;


/* ------------------------------------------------------------------

	Function: error
	Description: print a message to stderr and exit with an error
	Parameters: they work the same way as printf(3)
	Return value: none
	Notes: none

------------------------------------------------------------------ */

void error(const char *fmt, ...)
{
        char buf[1000];
        va_list ap;

        va_start(ap, fmt);

        vsnprintf(buf, 1000, fmt, ap);

        va_end(ap);

        cerr << buf << endl;

        exit(1);
}


/* ------------------------------------------------------------------

	Function: debug
	Description: print a debug message to stderr
	Parameters: they work the same way as printf(3)
	Return value: none
	Notes: the compiler macro DEBUG has to be defined for this
		function to do something

------------------------------------------------------------------ */

void debug(const char *fmt, ...)
{
#ifdef DEBUG
        char buf[1000];
        va_list ap;

        va_start(ap, fmt);

        vsnprintf(buf, 1000, fmt, ap);

        va_end(ap);

        cerr << "debug : " << buf << endl;
#endif
}
