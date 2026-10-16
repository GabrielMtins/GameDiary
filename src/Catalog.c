#include "App.h"
#include "SqlCmds.h"

enum InputState {
	CATALOG_INPUT_NAV,
	CATALOG_INPUT_DELETE_CONFIRM
};

static const int cols_width[] = {
	37,
	20,
	15,
	8,
};

static const char * Catalog_GetSelectedGameName(App *app, CatalogState *state);
static void Catalog_UpdateTable(App *app, CatalogState *state);
static void Catalog_DrawDeleteConfirmation(App *app);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_HandleInputNav(App *app, CatalogState *state);
static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);
static void Catalog_HandleDeleteQuery(App *app, CatalogState *state);

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

	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;
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

	Table *table = &state->table_data.working_table;
	bool is_table_valid = table->nrow > 1;


	switch(app->ch) {
		case 'a':
			app->next_state = STATE_ADDGAME;
			break;

		case 'i':
			if(!is_table_valid) break;

			AddLog_SetGame(app, Catalog_GetSelectedGameName(app, state));
			app->next_state = STATE_ADDLOG;
			break;

		case 'd':
			if(!is_table_valid) break;

			state->input_state = CATALOG_INPUT_DELETE_CONFIRM;
			break;

		case 'c':
		case 'r':
			if(!is_table_valid) break;

			app->next_state = STATE_UPDATEGAME;

			AddGame_SetUpdateData(
					app,
					&table->str[table->ncol * state->table_data.selected_row]
					);
			break;
	}
}

static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state) {
	switch(app->ch) {
		case 'n':
		case KEY_ESCAPE:
			state->input_state = CATALOG_INPUT_NAV;
			break;

		case 'y':
		case '\n':
			state->input_state = CATALOG_INPUT_NAV;

			Catalog_HandleDeleteQuery(app, state);
			Catalog_UpdateTable(app, state);
			break;
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

static void Catalog_HandleDeleteQuery(App *app, CatalogState *state) {
	char *cmd;

	cmd = sqlite3_mprintf(sql_cmd_delete_game, Catalog_GetSelectedGameName(app, state));
	
	sqlite3_exec(
			app->db,
			cmd,
			NULL,
			NULL,
			NULL
			);

	sqlite3_free(cmd);
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
