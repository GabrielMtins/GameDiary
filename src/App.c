#include "App.h"

#include "cmds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void App_InitDb(App *app) {
	sqlite3_open("data.db", &app->db);

	sqlite3_exec(
			app->db,
			create_tables_cmd,
			NULL,
			NULL,
			NULL
			);

	/*
	sqlite3_exec(
			app->db,
			"INSERT INTO GameTable (Name, Genres, Platform, Year) VALUES ('Super Mario World', 'Platform', 'SNES', '1991');",
			NULL,
			NULL,
			NULL
			);
			*/

	/*
	Table_GetSqlTable(
			&app->test_table,
			app->db,
			get_game_table_cmd
			);
			*/
}

static void App_Loop(App *app) {
	refresh();

	#define EXPAND_AS_CASE(type, state, update) case type: update(app); break;
	switch(app->current_state) {
		FOR_STATE_LIST_DO(EXPAND_AS_CASE)

		default:
			break;
	}
	#undef EXPAND_AS_CASE

	wrefresh(app->content_window.win);
	wrefresh(app->top_window.win);
	wrefresh(app->bottom_window.win);

	app->ch = getch();

	app->prev_state = app->current_state;
	app->current_state = app->next_state;
}

void App_Init(App *app) {
	memset(app, 0, sizeof(App));
	App_InitDb(app);

	initscr();
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	App_BuildWindow(app);

	#define EXPAND_AS_MALLOC(type, state, update) app->states[type] = malloc(sizeof(state));
	FOR_STATE_LIST_DO(EXPAND_AS_MALLOC)
	#undef EXPAND_AS_MALLOC

	app->quit = 0;
	app->ch = 0;

	app->prev_state = STATE_NONE;
	app->current_state = STATE_START;
	app->next_state = STATE_START;
}

void App_BuildWindow(App *app) {
	int width, height;
	getmaxyx(stdscr, height, width);
	curs_set(0);

	Window_Create(&app->top_window, 0, 0, width, 1);
	Window_Create(&app->bottom_window, 0, height - 1, width, 1);
	Window_Create(&app->content_window, 0, 1, width, height - 2);
}

void App_Run(App *app) {
	while(!app->quit) {
		App_Loop(app);
	}
}

void App_End(App *app) {
	for(int i = 0; i < NUM_STATES; i++) free(app->states[i]);

	Window_Destroy(&app->top_window);
	Window_Destroy(&app->bottom_window);
	Window_Destroy(&app->content_window);

	endwin();
}

void StartState_Update(App *app) {
	CatalogState *state = (CatalogState *) app->states[STATE_CATALOG];

	Table_GetSqlTable(
			&state->working_table,
			app->db,
			get_game_table_cmd
			);

	app->next_state = STATE_ADDGAME;
	Menubar_Update(app);
	Catalog_Update(app);
}
