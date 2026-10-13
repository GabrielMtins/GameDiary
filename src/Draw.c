#include "Draw.h"

#include <string.h>

void wprintbynumch(WINDOW *win, const char *str, int num_ch) {
	int length = strlen(str);

	for(int i = 0; i < num_ch; i++) {
		if(i >= length) {
			waddch(win, ' ');
		} else {
			waddch(win, (int) str[i]);
		}
	}
}
