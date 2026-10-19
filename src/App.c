#include "App.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGO_WINDOW_WIDTH 11

static void App_HandleResize(App *app);
static void App_InitState(App *app, int state);
static void App_UpdateState(App *app, int state);
static void App_BuildWindow(App *app);
static void App_RefreshWindows(App *app);
static void App_Loop(App *app);

static void App_HandleResize(App *app) {
	App_BuildWindow(app);

	wclear(app->content_window);
	Logo_Update(app);
}

static void App_InitState(App *app, int state) {
	#define EXPAND_AS_CASE(type, state, init, update) case type: init(app); break;
	switch(state) {
		FOR_STATE_LIST(EXPAND_AS_CASE)

		default:
			break;
	}
	#undef EXPAND_AS_CASE
}

static void App_UpdateState(App *app, int state) {
	#define EXPAND_AS_CASE(type, state, init, update) case type: update(app); break;
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

	if(app->top_window != NULL) delwin(app->top_window);
	if(app->content_window != NULL) delwin(app->content_window);
	if(app->logo_window != NULL) delwin(app->logo_window);
	if(app->bottom_window != NULL) delwin(app->bottom_window);

	app->top_window = newwin(1, width, 0, 0);
	app->content_window = newwin(height - 2, width, 1, 0);
	app->logo_window = newwin(1, width, height - 1, 0);
	app->bottom_window = newwin(1, width - LOGO_WINDOW_WIDTH, height - 1, LOGO_WINDOW_WIDTH);
}

static void App_RefreshWindows(App *app) {
	refresh();
	wrefresh(app->content_window);
	wrefresh(app->top_window);
	wrefresh(app->bottom_window);
	wrefresh(app->logo_window);
}

static void App_Loop(App *app) {
	if(app->ch == KEY_RESIZE) {
		App_HandleResize(app);
	}

	App_UpdateState(app, app->current_state);
	Menubar_Update(app);
	App_RefreshWindows(app);

	app->prev_state = app->current_state;
	app->current_state = app->next_state;

	if(app->prev_state != app->current_state) {
		app->ch = 0;
		wclear(app->content_window);
		App_ClearBottomWindow(app);
		App_InitState(app, app->current_state);
		App_UpdateState(app, app->current_state);
		App_RefreshWindows(app);
	}

	if(!app->quit) {
		app->prev_ch = app->ch;
		app->ch = getch();
	}
}

void App_Init(App *app) {
	memset(app, 0, sizeof(App));
	DB_Connect(&app->db);
	DB_InsertTestGames(app->db, 10);

	initscr();
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	set_escdelay(25);

	#define EXPAND_AS_MALLOC(type, state, init, update) app->states[type] = calloc(1, sizeof(state));
	FOR_STATE_LIST(EXPAND_AS_MALLOC)
	#undef EXPAND_AS_MALLOC

	app->quit = 0;
	app->ch = 0;

	app->prev_state = STATE_NONE;
	app->current_state = STATE_CATALOG;
	app->next_state = app->current_state;

	#define EXPAND_AS_INIT(type, state, init, update) init(app);
	FOR_STATE_LIST(EXPAND_AS_INIT)
	#undef EXPAND_AS_MALLOC

	App_InitState(app, app->current_state);
	App_HandleResize(app);
}

void App_Run(App *app) {
	while(!app->quit) {
		App_Loop(app);
	}
}

void App_End(App *app) {
	for(int i = 0; i < NUM_STATES; i++) free(app->states[i]);

	delwin(app->top_window);
	delwin(app->bottom_window);
	delwin(app->content_window);
	delwin(app->logo_window);

	endwin();

	sqlite3_close(app->db);
}

void App_ClearBottomWindow(App *app) {
	WINDOW *win = app->bottom_window;

	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_BOTTOM_BAR);

	Widget_Space(win, -1);
}

bool App_IsNewState(App *app) {
	return app->prev_state != app->current_state;
}

void State_NullInit(App *app) {
	(void) app;
}

void Logo_Update(App *app) {
	WINDOW *win = app->logo_window;

	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_BOTTOM_BAR);

	Widget_Label(
			win,
			" GameDiary ",
			-1
			);

	Widget_Space(win, -1);
}
