#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>
#include <curses.h>

#include "App.h"

int main(void) {
	App *app;

	app = (App *) malloc(sizeof(App));

	App_Init(app);
	App_Run(app);
	App_End(app);

	return 0;
}

/*
void example(void) {
	sqlite3 *db;
	char *errmsg;
	char *new_cmd = (char *) malloc(4096);
	int rc;

	sqlite3_open("data.db", &db);

	rc = sqlite3_exec(
			db,
			create_tables_cmd,
			NULL,
			NULL,
			&errmsg
			);

	if(rc != SQLITE_OK) {
		sqlite3_free(errmsg);
	}

	sprintf(new_cmd, insert_game_cmd, "Mega Man X2", "Platform", "Super Nintendo", 1994);

	rc = sqlite3_exec(
			db,
			new_cmd,
			NULL,
			NULL,
			&errmsg
			);

	if(rc != SQLITE_OK) {
		printf("%s\n", errmsg);
		sqlite3_free(errmsg);
	}

	sqlite3_close(db);
}
*/
