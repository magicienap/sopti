#include <stdarg.h>
#include <iostream>

#include "globals.hpp"

using namespace std;

void error(const char *fmt, ...)
{
        char buf[1000];
        va_list ap;

        va_start(ap, fmt);

        vsnprintf(buf, 1000, fmt, ap);

        va_end(ap);

        cerr << "error : " << buf << endl;

        abort();
}

void debug(const char *fmt, ...)
{
        char buf[1000];
        va_list ap;

        va_start(ap, fmt);

        vsnprintf(buf, 1000, fmt, ap);

        va_end(ap);

        cerr << "debug : " << buf << endl;
}
