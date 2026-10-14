#ifndef TABLE_H
#define TABLE_H

#include <sqlite3.h>
#include "Window.h"

typedef struct Table {
	char **str;
	int ncol, nrow;
} Table;

void Table_GetSqlTable(Table *table, sqlite3 *db, const char *cmd);

void Table_Destroy(Table *table);

void Table_PrintRowOnWindow(Window *window, Table *table, int window_y, int table_row);

#endif 
