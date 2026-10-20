#include "Widgets.h"
#include "App.h"

#include <ctype.h>

#define MAX_DATE_LEN 10

static void LineWidgetData_ConvertToDate(LineWidgetData *data);
static bool LineWidgetData_VerifyType(LineWidgetData *data, int ch);
static void LineWidgetData_AddCharacter(LineWidgetData *data, int ch);
static void LineWidgetData_RemoveCharacter(LineWidgetData *data);

static void LineWidgetData_ConvertToDate(LineWidgetData *data) {
	char numbers[16];
	int count = 0;
	int actual_str_len = 0;

	for(int i = 0; i < data->len; i++) {
		if(isdigit(data->str[i])) {
			numbers[count++] = data->str[i];
		}
	}

	for(int i = 0; i < count; i++) {
		if(i == 2 || i == 4) {
			data->str[actual_str_len++] = '/';
		}

		data->str[actual_str_len++] = numbers[i];
	}

	data->cursor_pos += actual_str_len - data->len;
	data->len = actual_str_len;
	data->str[data->len] = '\0';
}

static bool LineWidgetData_VerifyType(LineWidgetData *data, int ch) {
	switch(data->input_type) {
		default:
		case INPUT_TYPE_TEXT:
			return true;
			break;

		case INPUT_TYPE_DATE:
			if(data->len < MAX_DATE_LEN && isdigit(ch)) {
				return true;
			}
			break;
			
		case INPUT_TYPE_NUMBER:
			if(isdigit(ch)) {
				return true;
			}
			break;
	}

	return false;
}

static void LineWidgetData_AddCharacter(LineWidgetData *data, int ch) {
	if(data->len >= MAX_LINE_SIZE) return;
	if(!LineWidgetData_VerifyType(data, ch)) return;

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
		if(i == data->cursor_pos) {
			Style_SetAttr(win, data->cursor_color);
		} else {
			Style_SetAttr(win, data->bg_color);
		}

		if(i - data->offset >= data->width) {
			return;
		}

		waddch(win, data->str[i]);
	}

	count = data->len - data->offset;

	if(data->cursor_pos >= data->len) {
		Style_SetAttr(win, data->cursor_color);
		waddch(win, ' ');
		count++;
	}

	Style_SetAttr(win, data->bg_color);

	for(; count < data->width; count++) {
		waddch(win, ' ');
	}
}

void LineWidgetData_Init(LineWidgetData *data) {
	data->len = 0;
	data->str[0] = '\0';
	data->cursor_pos = 0;
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

	if(data->input_type == INPUT_TYPE_DATE) {
		LineWidgetData_ConvertToDate(data);
	}
}

void LineWidgetData_ApplyLimit(LineWidgetData *data) {
	data->str[data->len] = '\0';
}
