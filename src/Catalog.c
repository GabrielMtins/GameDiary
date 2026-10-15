#include "App.h"
#include "SqlCmds.h"

enum InputState {
	CATALOG_INPUT_NAV,
	CATALOG_INPUT_DELETE_CONFIRM
};

static const char * Catalog_GetSelectedGameName(App *app, CatalogState *state);
static void Catalog_UpdateTable(App *app, CatalogState *state);
static void Catalog_DrawClear(App *app, CatalogState *state);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_DrawDeleteConfirmation(App *app);
static void Catalog_HandleInputNav(App *app, CatalogState *state);
static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);
static void Catalog_HandleDelete(App *app, CatalogState *state);

static const char * Catalog_GetSelectedGameName(App *app, CatalogState *state) {
	return state->working_table.str[
		state->row_selected * state->working_table.ncol
	];
}

static void Catalog_UpdateTable(App *app, CatalogState *state) {
	Table_GetSqlTable(
			&state->working_table,
			app->db,
			sql_cmd_get_game_table
			);
}

static void Catalog_DrawClear(App *app, CatalogState *state) {
	WINDOW *win = app->content_window.win;

	wclear(win);

	wattrset(win, A_REVERSE);
	wmove(win, 0, 0);

	if(state->working_table.nrow == 1) {
		Widget_Label(win, "  There are no games on the catalog.", -1);
		Widget_Space(win, -1);
	} else {
		Table_PrintRowOnWindow(
				&app->content_window,
				&state->working_table,
				0,
				0
				);

	}
}

static void Catalog_DrawTable(App *app, CatalogState *state) {
	WINDOW *win = app->content_window.win;

	state->start_row = 1;

	for(int line = 1; line < app->content_window.height; line++) {
		int current_row = (line - 1) + state->start_row;

		if(current_row >= state->working_table.nrow) {
			wmove(win, line, 0);
			wattrset(win, A_NORMAL);
			Widget_Space(win, -1);
			continue;
		}

		if(current_row == state->row_selected) {
			wattrset(win, A_REVERSE | A_BOLD | A_DIM);
		} else {
			wattrset(win, A_NORMAL);
		}

		Table_PrintRowOnWindow(
				&app->content_window,
				&state->working_table,
				line,
				current_row
				);
	}
}

static void Catalog_DrawDeleteConfirmation(App *app) {
	WINDOW *win = app->bottom_window.win;
	
	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);
	Widget_Label(win, " Delete game? Y/n", -1);
	Widget_Space(win, -1);
}

static void Catalog_HandleInputNav(App *app, CatalogState *state) {
	if(app->ch == 'j') {
		state->row_selected++;
	}

	if(app->ch == 'k') {
		state->row_selected--;
	}

	/*
	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_MENUBAR;
	}
	*/

	LIMIT_VAR(state->row_selected, 1, state->working_table.nrow);

	if(app->ch == 'd' && state->working_table.nrow > 1) {
		state->input_state = CATALOG_INPUT_DELETE_CONFIRM;
	}

	if(app->ch == 'a') {
		app->next_state = STATE_ADDGAME;
	}

	if(app->ch == 'i') {
		AddLog_SetGame(app, Catalog_GetSelectedGameName(app, state));
		app->next_state = STATE_ADDLOG;
	}
}

static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state) {
	if(app->ch == 'n' || app->ch == KEY_ESCAPE) {
		state->input_state = CATALOG_INPUT_NAV;
	}

	if(app->ch == 'y' || app->ch == '\n') {
		state->input_state = CATALOG_INPUT_NAV;

		Catalog_HandleDelete(app, state);
		if(state->row_selected > 1) state->row_selected--;

		if(state->working_table.nrow == 1) {
			Catalog_DrawClear(app, state);
		}
	}
}

static void Catalog_HandleInput(App *app, CatalogState *state) {
	switch(state->input_state) {
		case CATALOG_INPUT_NAV:
			Catalog_HandleInputNav(app, state);
			break;

		case CATALOG_INPUT_DELETE_CONFIRM:
			Catalog_HandleInputDeleteConfirm(app, state);
			break;
	}
}

static void Catalog_HandleDelete(App *app, CatalogState *state) {
	char *cmd = NULL;
	int len;
	
	const char *game_name = state->working_table.str[state->row_selected * state->working_table.ncol];

	len = snprintf(cmd, 0, sql_cmd_delete_game,
			game_name
			) + 1;

	cmd = (char *) malloc(len);

	snprintf(cmd, len, sql_cmd_delete_game,
			game_name
			);

	sqlite3_exec(
			app->db,
			cmd,
			NULL,
			NULL,
			NULL
			);

	free(cmd);

	Catalog_UpdateTable(app, state);
}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	if(app->ch == KEY_RESIZE) {
		Catalog_DrawClear(app, state);
	}

	if(App_IsNewState(app)) {
		Catalog_UpdateTable(app, state);
		Catalog_DrawClear(app, state);

		app->content_window.owner_state = STATE_CATALOG;
		state->input_state = CATALOG_INPUT_NAV;
		state->row_selected = 1;
	}

	Catalog_HandleInput(app, state);

	if(state->input_state == CATALOG_INPUT_DELETE_CONFIRM) {
		Catalog_DrawDeleteConfirmation(app);
	} else {
		App_ClearBottomWindow(app);
	}

	Catalog_DrawTable(app, state);
}
