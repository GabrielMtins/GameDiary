#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdbool.h>

char * safe_sprintf(const char *format, ...);

bool util_isdecimal(const char *tmp);

#endif
