#include "Widgets.h"
#include "Draw.h"

#include <string.h>
#include <ctype.h>

void Widget_Space(WINDOW *win, int width) {
	if(width == -1) {
		int startx, starty, endx, endy;

		getyx(win, starty, startx);
		getmaxyx(win, endy, endx);

		width = endx - startx;

		(void) starty;
		(void) endy;
	}

	for(int i = 0; i < width; i++) {
		waddch(win, ' ');
	}
}

void Widget_Label(WINDOW *win, const char *label, int width) {
	if(width < 0) width = strlen(label);

	wprintbynumch(win, label, width);
}

void Widget_Button(WINDOW *win, const char *label, int width, int selected) {
	if(selected) {
		wattrset(win, A_REVERSE);
	} else {
		wattrset(win, A_REVERSE | A_DIM);
	}

	Widget_Label(win, label, width);
}

void Widget_InputLine(WINDOW *win, const LineWidgetData *data) {
	int count;

	for(int i = data->offset; i < data->len; i++) {
		if(i - data->offset >= data->width) {
			return;
		}

		if(i == data->cursor) {
			wattron(win, A_UNDERLINE | A_BOLD);
		} else {
			wattroff(win, A_UNDERLINE | A_BOLD);
		}

		waddch(win, data->str[i]);
	}

	wattroff(win, A_UNDERLINE | A_BOLD);

	count = data->len - data->offset;

	for(; count < data->width; count++) {
		waddch(win, ' ');
	}
}

void LineWidgetData_HandleInput(LineWidgetData *data, int ch) {
	if(isprint(ch)) {
		data->str[data->cursor] = (char) ch;

		if(data->cursor == data->len) {
			data->len++;
		}

		data->cursor++;
	}

	if(ch == KEY_LEFT) {
		data->cursor--;
	}
	if(ch == KEY_RIGHT) {
		data->cursor++;
	}

	if(ch == KEY_BACKSPACE && data->len > 0) {
		for(int i = data->cursor; i < data->len - 1; i++) {
			data->str[i] = data->str[i + 1];
		}

		data->len--;
		data->cursor--;
	}

	if(data->cursor > data->len) data->cursor = data->len;
	if(data->cursor < 0) data->cursor = 0;

	if(data->cursor < data->offset) {
		data->offset = data->cursor;
	}

	/*
	if(data->cursor >= data->offset + data->width) {
		data->offset = data->cursor - data->width + 1;
	}
	*/
}
