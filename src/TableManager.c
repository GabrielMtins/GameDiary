#include "App.h"

typedef enum {
	MANAGER_INPUT_NAV,
	MANAGER_INPUT_SEARCH,
	MANAGER_INPUT_DELETE,
} TableManagerInputState;

static void TableManager_UpdateTable(App *app, TableManagerState *state);
static void TableManager_DeleteItem(App *app, TableManagerState *state);
static void TableManager_HandleInputNav(App *app, TableManagerState *state);
static void TableManager_HandleInputSearch(App *app, TableManagerState *state);
static void TableManager_HandleInputDeleteConfirm(App *app, TableManagerState *state);
static void TableManager_HandleInput(App *app, TableManagerState *state);
static void TableManager_DrawFilterBar(App *app, TableManagerState *state);
static void TableManager_DrawDeleteConfirmation(App *app, TableManagerState *state);
static void TableManager_DrawSearch(App *app, TableManagerState *state);
static void TableManager_DrawTable(App *app, TableManagerState *state);

static void TableManager_UpdateTable(App *app, TableManagerState *state) {
	if(state->update_table_callback != NULL) {
		state->update_table_callback(app, state);
	}
}

static void TableManager_DeleteItem(App *app, TableManagerState *state) {
	if(state->delete_item_callback != NULL) {
		state->delete_item_callback(app, state);
	}
}

static void TableManager_HandleInputNav(App *app, TableManagerState *state) {
	Table *table = &state->table_data.working_table;
	bool is_table_valid = table->nrow > 1;

	switch(app->ch) {
		case 'd':
			if(!is_table_valid) break;

			state->input_state = MANAGER_INPUT_DELETE;
			break;

		case 'f':
			LineWidgetData_Init(&state->search_data);
			state->input_state = MANAGER_INPUT_SEARCH;
			break;

		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			TableManager_UpdateTable(app, state);
			break;
	}
}

static void TableManager_HandleInputSearch(App *app, TableManagerState *state) {
	app->block_menubar = true;
	LineWidgetData_HandleInput(&state->search_data, app->ch);

	switch(app->ch) {
		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			state->input_state = MANAGER_INPUT_NAV;
			break;

		case '\n':
			state->input_state = MANAGER_INPUT_NAV;
			TableManager_UpdateTable(app, state);
			break;
	}
}

static void TableManager_HandleInputDeleteConfirm(App *app, TableManagerState *state) {
	switch(app->ch) {
		case 'n':
		case KEY_ESCAPE:
			state->input_state = MANAGER_INPUT_NAV;
			break;

		case 'y':
		case '\n':
			state->input_state = MANAGER_INPUT_NAV;

			TableManager_UpdateTable(app, state);
			TableManager_DeleteItem(app, state);
			break;
	}
}

static void TableManager_HandleInput(App *app, TableManagerState *state) {
	switch(state->input_state) {
		case MANAGER_INPUT_NAV:
			TableManager_HandleInputNav(app, state);
			break;

		case MANAGER_INPUT_DELETE:
			TableManager_HandleInputDeleteConfirm(app, state);
			break;

		case MANAGER_INPUT_SEARCH:
			TableManager_HandleInputSearch(app, state);
			break;
	}
}

static void TableManager_DrawFilterBar(App *app, TableManagerState *state) {
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

static void TableManager_DrawDeleteConfirmation(App *app, TableManagerState *state) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_CONFIRM_MSG);
	Widget_Label(win, " Delete game? Y/n", -1);
	Widget_Space(win, -1);
}

static void TableManager_DrawSearch(App *app, TableManagerState *state) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_SEARCH_BG);
	Widget_Label(win, " Search: ", -1);

	state->search_data.width = getmaxx(win) - getcurx(win);
	Widget_InputLine(win, &state->search_data);
}

static void TableManager_DrawTable(App *app, TableManagerState *state) {
	WINDOW *win = app->content_window;
	
	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	switch(state->input_state) {
		case MANAGER_INPUT_NAV:
			TableManager_DrawFilterBar(app, state);
			break;

		case MANAGER_INPUT_DELETE:
			TableManager_DrawDeleteConfirmation(app, state);
			break;

		case MANAGER_INPUT_SEARCH:
			TableManager_DrawSearch(app, state);
			break;
	}
}

void TableManager_Init(App *app) {
	TableManagerState *state = app->states[app->current_state];

	state->input_state = MANAGER_INPUT_NAV;
	
	/*
	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;
	*/

	state->search_data.bg_color = STYLE_SEARCH_BG;
	state->search_data.cursor_color = STYLE_SEARCH_CURSOR;

	TableManager_UpdateTable(app, state);
}

void TableManager_Update(App *app) {
	TableManagerState *state = app->states[app->current_state];

	TableManager_HandleInput(app, state);

	TableManager_DrawTable(app, state);
}
