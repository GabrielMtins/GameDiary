#include "Util.h"

#include <stdio.h>
#include <stdlib.h>

char * safe_sprintf(const char *format, ...) {
	char *tmp = NULL;
	size_t len = 0;

	va_list args, copy;
	va_start(args, format);
	va_copy(copy, args);

	len = vsnprintf(tmp, 0, format, args) + 1;
	tmp = (char *) malloc(len);

	vsnprintf(tmp, len, format, copy);

	va_end(args);
	va_end(copy);

	return tmp;
}
