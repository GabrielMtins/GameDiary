#include "App.h"
#include <string.h>

enum InputState {
	CATALOG_INPUT_NAV,
	CATALOG_INPUT_SEARCH,
	CATALOG_INPUT_DELETE_CONFIRM
};

static const int cols_width[] = {
	37,
	20,
	15,
	8,
};

static void Catalog_UpdateTable(App *app, CatalogState *state);
static void Catalog_DrawFilterBar(App *app, CatalogState *state);
static void Catalog_DrawDeleteConfirmation(App *app);
static void Catalog_DrawSearch(App *app, CatalogState *state);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_HandleInputNav(App *app, CatalogState *state);
static void Catalog_HandleInputSearch(App *app, CatalogState *state);
static void Catalog_HandleInputDeleteConfirm(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);

static void Catalog_UpdateTable(App *app, CatalogState *state) {
	DB_SearchGame(
			app->db,
			&state->table_data.working_table,
			state->search_data.str
			);
}

static void Catalog_DrawDeleteConfirmation(App *app) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_CONFIRM_MSG);
	Widget_Label(win, " Delete game? Y/n", -1);
	Widget_Space(win, -1);
}

static void Catalog_DrawFilterBar(App *app, CatalogState *state) {
	WINDOW *win = app->bottom_window;

	if(state->search_data.len == 0) {
		App_ClearBottomWindow(app);
		return;
	}

	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_FILTER_MSG);
	Widget_Label(win, " Filtering by name: ", -1);
	Widget_Label(win, state->search_data.str, -1);
	Widget_Space(win, -1);
}

static void Catalog_DrawSearch(App *app, CatalogState *state) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_SEARCH_BG);
	Widget_Label(win, " Search: ", -1);

	state->search_data.width = getmaxx(win) - getcurx(win);
	Widget_InputLine(win, &state->search_data);
}

static void Catalog_DrawTable(App *app, CatalogState *state) {
	WINDOW *win;

	win = app->content_window;

	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	switch(state->input_state) {
		case CATALOG_INPUT_NAV:
			Catalog_DrawFilterBar(app, state);
			break;

		case CATALOG_INPUT_DELETE_CONFIRM:
			Catalog_DrawDeleteConfirmation(app);
			break;

		case CATALOG_INPUT_SEARCH:
			Catalog_DrawSearch(app, state);
			break;
	}
}

static void Catalog_HandleInputNav(App *app, CatalogState *state) {
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

		case 'f':
			LineWidgetData_Init(&state->search_data);
			state->input_state = CATALOG_INPUT_SEARCH;
			break;

		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			Catalog_UpdateTable(app, state);
			break;
	}

	TableWidgetData_HandleInput(&state->table_data, app->ch, app->prev_ch);
}

static void Catalog_HandleInputSearch(App *app, CatalogState *state) {
	app->block_menubar = true;
	LineWidgetData_HandleInput(&state->search_data, app->ch);

	switch(app->ch) {
		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			state->input_state = CATALOG_INPUT_NAV;
			break;

		case '\n':
			state->input_state = CATALOG_INPUT_NAV;
			Catalog_UpdateTable(app, state);
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
	state->table_data.height = getmaxy(app->content_window);

	switch(state->input_state) {
		case CATALOG_INPUT_NAV:
			Catalog_HandleInputNav(app, state);
			break;

		case CATALOG_INPUT_SEARCH:
			Catalog_HandleInputSearch(app, state);
			break;

		case CATALOG_INPUT_DELETE_CONFIRM:
			Catalog_HandleInputDeleteConfirm(app, state);
			break;
	}

	TableWidgetData_ApplyLimits(&state->table_data);
}

void Catalog_Init(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	state->input_state = CATALOG_INPUT_NAV;
	
	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;

	state->search_data.bg_color = STYLE_SEARCH_BG;
	state->search_data.cursor_color = STYLE_SEARCH_CURSOR;

	Catalog_UpdateTable(app, state);
}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	Catalog_HandleInput(app, state);

	Catalog_DrawTable(app, state);
}
