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
			&table->row,
			&table->col,
			NULL
			);
}

void Table_Destroy(Table *table) {
	sqlite3_free_table(table->str);
}

void Table_PrintRowOnWindow(Window *window, Table *table, int window_y, int table_row) {
	const int num_characters_to_print =
		(window->width - 1 - 3 * table->col) / table->col;

	for(int i = 0; i < table->col; i++) {
		wmove(window->win, window_y, i * (num_characters_to_print + 3));
		waddch(window->win, '|');
		waddch(window->win, ' ');
		wprintbynumch(window->win, table->str[table_row * table->col + i], num_characters_to_print);
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
