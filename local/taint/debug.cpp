#include "debug.h"
#include <stdio.h>
#include <stdarg.h>

int d_print(int level, const char* format, ...)
{
    va_list argptr;

    if(level <= DEBUG_LEVEL)
    {
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }

    return 0x0;
}

int err_print(const char* format, ...)
{
    va_list argptr;

    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);

    return 0x0;
}
