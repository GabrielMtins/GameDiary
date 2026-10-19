#include "App.h"

static const int cols_width[] = {
	0,
	37,
	16,
	16,
	11,
};

enum InputState {
	LOGVIEW_INPUT_NAV,
	LOGVIEW_INPUT_DELETE,
	LOGVIEW_INPUT_SEARCH,
};

static void LogView_UpdateTable(App *app, LogViewState *state);
static void LogView_HandleInputNav(App *app, LogViewState *state);
static void LogView_HandleInputDelete(App *app, LogViewState *state);
static void LogView_HandleInputSearch(App *app, LogViewState *state);
static void LogView_HandleInput(App *app, LogViewState *state);
static void LogView_DrawFilterBar(App *app, LogViewState *state);
static void LogView_DrawSearch(App *app, LogViewState *state);
static void LogView_DrawTable(App *app, LogViewState *state);
static void LogView_DrawDeleteConfirmation(App *app, LogViewState *state);

static void LogView_UpdateTable(App *app, LogViewState *state) {
	DB_SearchLog(
			app->db,
			&state->table_data.working_table,
			state->search_data.str
			);
}

static void LogView_HandleInputNav(App *app, LogViewState *state) {
	TableWidgetData_HandleInput(
			&state->table_data,
			app->ch,
			app->prev_ch
			);

	bool is_table_valid = state->table_data.working_table.nrow > 1;

	switch(app->ch) {
		case 'd':
			if(!is_table_valid) break;
			state->input_state = LOGVIEW_INPUT_DELETE;
			break;

		case 'c':
		case 'r':
			if(!is_table_valid) break;
			app->next_state = STATE_UPDATELOG;

			AddLog_SetUpdateData(
					app,
					TableWidgetData_GetSelectedRow(&state->table_data)
					);
			break;

		case 'f':
			state->input_state = LOGVIEW_INPUT_SEARCH;
			LineWidgetData_Init(&state->search_data);
			break;
	}
}

static void LogView_HandleInputDelete(App *app, LogViewState *state) {
	switch(app->ch) {
		case 'n':
		case KEY_ESCAPE:
			state->input_state = LOGVIEW_INPUT_NAV;
			break;

		case '\n':
		case 'y':
			state->input_state = LOGVIEW_INPUT_NAV;

			DB_DeleteLog(
					app->db,
					TableWidgetData_GetSelectedRow(&state->table_data)[0]
					);

			LogView_UpdateTable(app, state);
			break;
	}
}

static void LogView_HandleInputSearch(App *app, LogViewState *state) {
	app->block_menubar = true;
	LineWidgetData_HandleInput(&state->search_data, app->ch);

	switch(app->ch) {
		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			state->input_state = LOGVIEW_INPUT_NAV;
			break;

		case '\n':
			state->input_state = LOGVIEW_INPUT_NAV;
			LogView_UpdateTable(app, state);
			break;
	}
}

static void LogView_HandleInput(App *app, LogViewState *state) {
	switch(state->input_state) {
		case LOGVIEW_INPUT_NAV:
			LogView_HandleInputNav(app, state);
			break;

		case LOGVIEW_INPUT_DELETE:
			LogView_HandleInputDelete(app, state);
			break;

		case LOGVIEW_INPUT_SEARCH:
			LogView_HandleInputSearch(app, state);
			break;
	}

	TableWidgetData_ApplyLimits(&state->table_data);
}

static void LogView_DrawFilterBar(App *app, LogViewState *state) {
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

static void LogView_DrawSearch(App *app, LogViewState *state) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_SEARCH_BG);
	Widget_Label(win, " Search: ", -1);

	state->search_data.width = getmaxx(win) - getcurx(win);
	Widget_InputLine(win, &state->search_data);
}

static void LogView_DrawTable(App *app, LogViewState *state) {
	WINDOW *win = app->content_window;

	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	switch(state->input_state) {
		case LOGVIEW_INPUT_DELETE:
			LogView_DrawDeleteConfirmation(app, state);
			break;

		case LOGVIEW_INPUT_NAV:
			LogView_DrawFilterBar(app, state);
			break;

		case LOGVIEW_INPUT_SEARCH:
			LogView_DrawSearch(app, state);
			break;
	}
}

static void LogView_DrawDeleteConfirmation(App *app, LogViewState *state) {
	WINDOW *win = app->bottom_window;
	(void) state;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_CONFIRM_MSG);
	Widget_Label(win, " Delete diary entry? Y/n", -1);
	Widget_Space(win, -1);
}

void LogView_Init(App *app) {
	LogViewState *state = app->states[STATE_LOGVIEW];

	state->input_state = LOGVIEW_INPUT_NAV;
	
	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;
	state->table_data.height = getmaxy(app->content_window);

	state->search_data.bg_color = STYLE_SEARCH_BG;
	state->search_data.cursor_color = STYLE_SEARCH_CURSOR;

	LogView_UpdateTable(app, state);
}

void LogView_Update(App *app) {
	LogViewState *state = (LogViewState *) app->states[STATE_LOGVIEW];

	LogView_HandleInput(app, state);
	LogView_DrawTable(app, state);
}
