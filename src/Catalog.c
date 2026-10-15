#include "App.h"
#include "SqlCmds.h"

enum InputState {
	CATALOG_INPUT_NAV,
	CATALOG_INPUT_DELETE_CONFIRM
};

static const char * Catalog_GetSelectedGameName(App *app, CatalogState *state);
static void Catalog_UpdateTable(App *app, CatalogState *state);
static void Catalog_DrawDeleteConfirmation(App *app);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_HandleInputNav(App *app, CatalogState *state);
static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);
static void Catalog_HandleDelete(App *app, CatalogState *state);

static const char * Catalog_GetSelectedGameName(App *app, CatalogState *state) {
	(void) app;

	return state->table_data.working_table.str[
		state->table_data.selected_row * state->table_data.working_table.ncol
	];
}

static void Catalog_UpdateTable(App *app, CatalogState *state) {
	Table_GetSqlTable(
			&state->table_data.working_table,
			app->db,
			sql_cmd_get_game_table
			);

	state->table_data.start_row = 1;

	state->table_data.msg_no_entries = "  There are no games under this category.";
}

static void Catalog_DrawDeleteConfirmation(App *app) {
	WINDOW *win = app->bottom_window.win;
	
	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);
	Widget_Label(win, " Delete game? Y/n", -1);
	Widget_Space(win, -1);
}

static void Catalog_DrawTable(App *app, CatalogState *state) {
	WINDOW *win;

	win = app->content_window.win;

	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	if(state->input_state == CATALOG_INPUT_DELETE_CONFIRM) {
		Catalog_DrawDeleteConfirmation(app);
	} else {
		App_ClearBottomWindow(app);
	}
}

static void Catalog_HandleInputNav(App *app, CatalogState *state) {
	TableWidgetData_HandleInput(&state->table_data, app->ch);

	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_MENUBAR;
	}

	if(app->ch == 'd' && state->table_data.working_table.nrow > 1) {
		state->input_state = CATALOG_INPUT_DELETE_CONFIRM;
	}

	if(app->ch == 'a') {
		app->next_state = STATE_ADDGAME;
	}

	if(app->ch == 'i' && state->table_data.working_table.nrow > 1) {
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
		Catalog_UpdateTable(app, state);
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
	const char *game_name = Catalog_GetSelectedGameName(app, state);
	
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
}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	if(App_IsNewState(app)) {
		Catalog_UpdateTable(app, state);

		app->content_window.owner_state = STATE_CATALOG;
		state->input_state = CATALOG_INPUT_NAV;
	}

	Catalog_HandleInput(app, state);

	Catalog_DrawTable(app, state);
}
