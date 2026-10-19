#include "Widgets.h"
#include "App.h"

#include <string.h>
#include <ctype.h>

void Widget_Space(WINDOW *win, int width) {
	if(width == -1) {
		width = getmaxx(win) - getcurx(win);
	}

	for(int i = 0; i < width; i++) {
		waddch(win, ' ');
	}
}

void Widget_Label(WINDOW *win, const char *label, int width) {
	if(label == NULL) return;

	int len = strlen(label);
	int maxx = getmaxx(win);

	if(width < 0) width = len;
	if(width >= maxx) width = maxx - 1;

	for(int i = 0; i < width; i++) {
		if(i >= len) {
			waddch(win, ' ');
		} else {
			waddch(win, label[i]);
		}
	}
}

void Widget_Button(WINDOW *win, const char *label, int width, int selected) {
	if(selected) {
		Style_SetAttr(win, STYLE_BUTTON_SELECTED);
	} else {
		Style_SetAttr(win, STYLE_BUTTON_OFF);
	}

	Widget_Label(win, label, width);
}
