#include "App.h"

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

static void Catalog_UpdateTable(App *app, CatalogState *state);
static void Catalog_DrawDeleteConfirmation(App *app);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_HandleInputNav(App *app, CatalogState *state);
static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);

static void Catalog_UpdateTable(App *app, CatalogState *state) {
	DB_GetGameTable(app->db, &state->table_data.working_table);
}

static void Catalog_DrawDeleteConfirmation(App *app) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);
	Widget_Label(win, " Delete game? Y/n", -1);
	Widget_Space(win, -1);
}

static void Catalog_DrawTable(App *app, CatalogState *state) {
	WINDOW *win;

	win = app->content_window;

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

			AddLog_SetGame(app, TableWidgetData_GetSelectedRow(&state->table_data)[0]);
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
					TableWidgetData_GetSelectedRow(&state->table_data)
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

			DB_DeleteGame(
					app->db,
					TableWidgetData_GetSelectedRow(&state->table_data)[0]
					);

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

void Catalog_Init(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	state->input_state = CATALOG_INPUT_NAV;
	
	TableWidgetData_Init(&state->table_data);

	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;

	Catalog_UpdateTable(app, state);
}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	Catalog_HandleInput(app, state);

	Catalog_DrawTable(app, state);
}
