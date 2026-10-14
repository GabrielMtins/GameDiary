#include "Table.h"
#include "Draw.h"

#include <stddef.h>

void Table_GetSqlTable(Table *table, sqlite3 *db, const char *cmd) {
	if(table->str != NULL) {
		Table_Destroy(table);
	}

	sqlite3_get_table(
			db,
			cmd,
			&table->str,
			&table->nrow,
			&table->ncol,
			NULL
			);

	table->nrow++;
}

void Table_Destroy(Table *table) {
	sqlite3_free_table(table->str);
}

void Table_PrintRowOnWindow(Window *window, Table *table, int window_y, int table_row) {
	const int num_characters_to_print =
		(window->width - 1 - 3 * table->ncol) / table->ncol;

	for(int i = 0; i < table->ncol; i++) {
		wmove(window->win, window_y, i * (num_characters_to_print + 3));
		waddch(window->win, '|');
		waddch(window->win, ' ');
		wprintbynumch(window->win, table->str[table_row * table->ncol + i], num_characters_to_print);
		waddch(window->win, ' ');
	}

	{
		int y, x;
		getyx(window->win, y, x);
		(void) y;

		while(x++ < window->width - 1) {
			waddch(window->win, ' ');
		}
	}


	waddch(window->win, '|');
}
