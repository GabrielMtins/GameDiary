#include "App.h"

#include "SqlCmds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGO_WINDOW_WIDTH 11

static void App_InitDb(App *app);
static void App_HandleResize(App *app);
static void App_UpdateState(App *app, int state);
static void App_BuildWindow(App *app);
static void App_RefreshWindows(App *app);
static void App_Loop(App *app);

static void App_InitDb(App *app) {
	sqlite3_open("data.db", &app->db);

	sqlite3_exec(
			app->db,
			sql_cmd_create_tables,
			NULL,
			NULL,
			NULL
			);
}

static void App_HandleResize(App *app) {
	App_BuildWindow(app);

	App_UpdateState(app, app->bottom_window.owner_state);
	App_UpdateState(app, app->top_window.owner_state);
	App_UpdateState(app, app->content_window.owner_state);
	App_UpdateState(app, app->logo_window.owner_state);
}

static void App_UpdateState(App *app, int state) {
	#define EXPAND_AS_CASE(type, state, update) case type: update(app); break;
	switch(state) {
		FOR_STATE_LIST(EXPAND_AS_CASE)

		default:
			break;
	}
	#undef EXPAND_AS_CASE
}

static void App_BuildWindow(App *app) {
	int width, height;
	getmaxyx(stdscr, height, width);
	curs_set(0);

	Window_Create(&app->top_window, 0, 0, width, 1);
	Window_Create(&app->content_window, 0, 1, width, height - 2);
	Window_Create(&app->logo_window, 0, height - 1, width, 1);
	Window_Create(&app->bottom_window, LOGO_WINDOW_WIDTH, height - 1, width - LOGO_WINDOW_WIDTH, 1);
}

static void App_RefreshWindows(App *app) {
	refresh();
	wrefresh(app->content_window.win);
	wrefresh(app->top_window.win);
	wrefresh(app->bottom_window.win);
	wrefresh(app->logo_window.win);
}

static void App_Loop(App *app) {
	if(app->ch == KEY_RESIZE) {
		App_HandleResize(app);
	}

	App_UpdateState(app, app->current_state);
	App_RefreshWindows(app);

	app->prev_state = app->current_state;
	app->current_state = app->next_state;

	if(app->prev_state != app->current_state) {
		app->ch = 0;
		App_UpdateState(app, app->current_state);
		App_RefreshWindows(app);
	}

	app->ch = getch();
}

void App_Init(App *app) {
	memset(app, 0, sizeof(App));
	App_InitDb(app);

	initscr();
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	set_escdelay(25);

	#define EXPAND_AS_MALLOC(type, state, update) app->states[type] = calloc(1, sizeof(state));
	FOR_STATE_LIST(EXPAND_AS_MALLOC)
	#undef EXPAND_AS_MALLOC

	app->quit = 0;
	app->ch = 0;

	app->prev_state = STATE_NONE;
	app->current_state = STATE_CATALOG;
	app->next_state = app->current_state;

	app->content_window.owner_state = app->current_state;
	app->top_window.owner_state = STATE_MENUBAR;
	app->bottom_window.owner_state = STATE_NONE;
	app->logo_window.owner_state = STATE_LOGO;

	App_HandleResize(app);
	refresh();
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
	Window_Destroy(&app->logo_window);

	endwin();

	sqlite3_close(app->db);
}

void App_ClearBottomWindow(App *app) {
	WINDOW *win = app->bottom_window.win;

	wmove(win, 0, 0);
	wattrset(win, A_REVERSE);

	Widget_Space(win, -1);
}

bool App_IsNewState(App *app) {
	return app->prev_state != app->current_state;
}

void Logo_Update(App *app) {
	WINDOW *win = app->logo_window.win;

	wmove(win, 0, 0);
	wattrset(win, A_REVERSE);

	Widget_Label(
			win,
			" GameDiary ",
			-1
			);

	Widget_Space(win, -1);
}
