#include "Widgets.h"
#include "App.h"

#define CORRECT_COL_WIDTH(win, width) ((width) * (getmaxx(win)) / 80)

static void TableWidgetData_DrawLine(WINDOW *win, const TableWidgetData *data, int window_y, int table_row);

static void TableWidgetData_DrawLine(WINDOW *win, const TableWidgetData* data, int window_y, int table_row) {
	const Table *table = &data->working_table;

	int count = 0;

	wmove(win, window_y, 0);
	Widget_Space(win, -1);

	for(int i = 0; i < table->ncol; i++) {
		int current_col_width = CORRECT_COL_WIDTH(win, data->col_width_values[i]);

		if(current_col_width == 0) continue;


		wmove(win, window_y, count);
		waddch(win, ACS_VLINE);

		wmove(win, window_y, count + 2);

		Widget_Label(
				win,
				table->str[table_row * table->ncol + i],
				current_col_width - 3
				);

		count += current_col_width;
	}

	wmove(win, window_y, getmaxx(win) - 1);
	waddch(win, ACS_VLINE);
}

void Widget_Table(WINDOW *win, TableWidgetData *data) {
	const int start_y = getcury(win) + 1;
	const int end_y = getmaxy(win);

	wattrset(win, A_REVERSE);

	if(data->working_table.nrow > 1) {
		/* Draw column names. */
		TableWidgetData_DrawLine(
				win,
				data,
				0,
				0
				);

		if(data->selected_row >= data->working_table.nrow) {
			data->selected_row = data->working_table.nrow - 1;
		}
	} else {
		/* Show error message if there are no entries. */
		wmove(win, getcury(win), 0);
		Widget_Label(win, data->msg_no_entries, -1);
		Widget_Space(win, -1);
	}

	if(data->start_row < 1) data->start_row = 1;

	for(int line = start_y; line < end_y; line++) {
		int current_row = (line - start_y) + data->start_row;

		if(current_row >= data->working_table.nrow) {
			wmove(win, line, 0);
			wattrset(win, A_NORMAL);
			Widget_Space(win, -1);
			continue;
		}

		if(current_row == data->selected_row) {
			wattrset(win, A_REVERSE | A_BOLD);
		} else {
			wattrset(win, A_NORMAL);
		}

		TableWidgetData_DrawLine(
				win,
				data,
				line,
				current_row
				);
	}
}

void TableWidgetData_HandleInput(TableWidgetData *data, int ch) {
	switch(ch) {
		case 'j':
		case KEY_DOWN:
			data->selected_row++;
			break;

		case 'k':
		case KEY_UP:
			data->selected_row--;
			break;
	}

	LIMIT_VAR(data->selected_row, 1, data->working_table.nrow);
}

char ** TableWidgetData_GetSelectedRow(const TableWidgetData *data) {
	return &data->working_table.str[
		data->working_table.ncol * data->selected_row
	];
}
