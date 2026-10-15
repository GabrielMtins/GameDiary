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
