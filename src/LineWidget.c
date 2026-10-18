#include "Widgets.h"
#include "App.h"

#include <ctype.h>

static void LineWidgetData_AddCharacter(LineWidgetData *data, int ch);
static void LineWidgetData_RemoveCharacter(LineWidgetData *data);

static void LineWidgetData_AddCharacter(LineWidgetData *data, int ch) {
	if(data->len >= MAX_LINE_SIZE) return;

	for(int i = data->len; i >= data->cursor_pos + 1; i--) {
		data->str[i] = data->str[i - 1];
	}

	data->str[data->cursor_pos] = (char) ch;

	data->cursor_pos++;
	data->len++;

	data->str[data->len] = '\0';
}

static void LineWidgetData_RemoveCharacter(LineWidgetData *data) {
	if(data->len <= 0 || data->cursor_pos <= 0) {
		return;
	}

	for(int i = data->cursor_pos - 1; i < data->len - 1; i++) {
		data->str[i] = data->str[i + 1];
	}

	data->len--;
	data->cursor_pos--;

	data->str[data->len] = '\0';
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

void LineWidgetData_Init(LineWidgetData *data) {
	memset(data, 0, sizeof(LineWidgetData));

	data->bg_attron = A_NORMAL;
	data->cursor_attron = A_REVERSE;
}

void LineWidgetData_HandleInput(LineWidgetData *data, int ch) {
	if(isprint(ch)) {
		LineWidgetData_AddCharacter(data, ch);
	}

	switch(ch) {
		case KEY_LEFT:
			data->cursor_pos--;
			break;

		case KEY_RIGHT:
			data->cursor_pos++;
			break;

		case KEY_BACKSPACE:
			LineWidgetData_RemoveCharacter(data);
			break;
	}

	LIMIT_VAR(data->cursor_pos, 0, data->len + 1);

	if(data->cursor_pos < data->offset) {
		data->offset = data->cursor_pos;
	}

	if(data->cursor_pos >= data->offset + data->width) {
		data->offset = data->cursor_pos - data->width + 1;
	}
}
