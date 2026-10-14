#include "App.h"

#include "cmds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void App_InitDb(App *app);
static void App_HandleResize(App *app);
static void App_UpdateState(App *app, int state);
static void App_Loop(App *app);

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

static void App_HandleResize(App *app) {
	App_BuildWindow(app);

	App_UpdateState(app, app->bottom_window.owner_state);
	App_UpdateState(app, app->top_window.owner_state);
	App_UpdateState(app, app->content_window.owner_state);
}

static void App_UpdateState(App *app, int state) {
	#define EXPAND_AS_CASE(type, state, update) case type: update(app); break;
	switch(state) {
		FOR_STATE_LIST_DO(EXPAND_AS_CASE)

		default:
			break;
	}
	#undef EXPAND_AS_CASE
}

static void App_Loop(App *app) {
	refresh();

	App_UpdateState(app, app->current_state);

	wrefresh(app->content_window.win);
	wrefresh(app->top_window.win);
	wrefresh(app->bottom_window.win);

	app->ch = getch();

	if(app->ch == KEY_RESIZE) {
		App_HandleResize(app);
	}

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

	#define EXPAND_AS_MALLOC(type, state, update) app->states[type] = malloc(sizeof(state));
	FOR_STATE_LIST_DO(EXPAND_AS_MALLOC)
	#undef EXPAND_AS_MALLOC

	app->quit = 0;
	app->ch = 0;

	app->prev_state = STATE_NONE;
	app->current_state = STATE_ADDGAME;
	app->next_state = STATE_ADDGAME;

	app->content_window.owner_state = STATE_ADDGAME;
	app->top_window.owner_state = STATE_MENUBAR;
	app->bottom_window.owner_state = STATE_NONE;

	App_HandleResize(app);
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
