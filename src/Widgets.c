#include "Widgets.h"

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

	if(width < 0) width = len;

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
		wattrset(win, A_REVERSE);
	} else {
		wattrset(win, A_REVERSE | A_DIM);
	}

	Widget_Label(win, label, width);
}

void Widget_InputLine(WINDOW *win, const LineWidgetData *data) {
	int count;

	for(int i = data->offset; i < data->len; i++) {
		wattrset(win, (i == data->cursor_pos) ? data->cursor_attron : data->bg_attron);

		if(i - data->offset >= data->width) {
			return;
		}

		waddch(win, data->str[i]);
	}

	count = data->len - data->offset;

	if(data->cursor_pos >= data->len) {
		wattrset(win, data->cursor_attron);
		waddch(win, ' ');
		count++;
	}

	wattrset(win, data->bg_attron);

	for(; count < data->width; count++) {
		waddch(win, ' ');
	}
}

void LineWidgetData_HandleInput(LineWidgetData *data, int ch) {
	if(isprint(ch)) {
		data->len++;

		for(int i = data->len - 1; i >= data->cursor_pos + 1; i--) {
			data->str[i] = data->str[i - 1];
		}

		data->str[data->cursor_pos] = (char) ch;

		data->cursor_pos++;
	}

	if(ch == KEY_LEFT) {
		data->cursor_pos--;
	}
	if(ch == KEY_RIGHT) {
		data->cursor_pos++;
	}

	if(ch == KEY_BACKSPACE && data->len > 0 && data->cursor_pos > 0) {
		for(int i = data->cursor_pos - 1; i < data->len - 1; i++) {
			data->str[i] = data->str[i + 1];
		}

		data->len--;
		data->cursor_pos--;
	}

	if(data->cursor_pos > data->len) data->cursor_pos = data->len;
	if(data->cursor_pos < 0) data->cursor_pos = 0;

	if(data->cursor_pos < data->offset) {
		data->offset = data->cursor_pos;
	}

	if(data->cursor_pos >= data->offset + data->width) {
		data->offset = data->cursor_pos - data->width + 1;
	}
}
