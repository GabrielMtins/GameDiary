#include "Widgets.h"
#include "App.h"

static void TableWidget_DrawLine(WINDOW *win, const Table *table, int window_y, int table_row);

static void TableWidget_DrawLine(WINDOW *win, const Table *table, int window_y, int table_row) {
	const int num_characters_to_print =
		(getmaxx(win) - 1 - 3 * table->ncol) / table->ncol;

	for(int i = 0; i < table->ncol; i++) {
		wmove(win, window_y, i * (num_characters_to_print + 3));
		waddch(win, '|');
		waddch(win, ' ');
		Widget_Label(win, table->str[table_row * table->ncol + i], num_characters_to_print);
		waddch(win, ' ');
	}

	{
		int x = getcurx(win);

		while(x++ < getmaxx(win) - 1) {
			waddch(win, ' ');
		}
	}


	waddch(win, '|');
}

void Widget_Table(WINDOW *win, const TableWidgetData *data) {
	const int start_y = getcury(win) + 1;
	const int end_y = getmaxy(win);

	wattrset(win, A_REVERSE);

	if(data->working_table.nrow > 1) {
		/* Draw column names. */
		TableWidget_DrawLine(
				win,
				&data->working_table,
				0,
				0
				);
	} else {
		/* Show error message if there are no entries. */
		wmove(win, getcury(win), 0);
		Widget_Label(win, data->msg_no_entries, -1);
		Widget_Space(win, -1);
	}

	for(int line = start_y; line < end_y; line++) {
		int current_row = (line - start_y) + data->start_row;

		if(current_row >= data->working_table.nrow) {
			wmove(win, line, 0);
			wattrset(win, A_NORMAL);
			Widget_Space(win, -1);
			continue;
		}

		if(current_row == data->selected_row) {
			wattrset(win, A_REVERSE | A_BOLD | A_DIM);
		} else {
			wattrset(win, A_NORMAL);
		}

		TableWidget_DrawLine(
				win,
				&data->working_table,
				line,
				current_row
				);
	}
}

void TableWidgetData_HandleInput(TableWidgetData *data, int ch) {
	if(ch == 'j' || ch == KEY_DOWN) {
		data->selected_row++;
	}

	if(ch == 'k' || ch == KEY_UP) {
		data->selected_row--;
	}

	LIMIT_VAR(data->selected_row, 1, data->working_table.nrow);
}
