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
};

static void LogView_UpdateTable(App *app, LogViewState *state);
static void LogView_HandleInputNav(App *app, LogViewState *state);
static void LogView_HandleInputDelete(App *app, LogViewState *state);
static void LogView_HandleInput(App *app, LogViewState *state);
static void LogView_DrawTable(App *app, LogViewState *state);
static void LogView_DrawDeleteConfirmation(App *app, LogViewState *state);

static void LogView_UpdateTable(App *app, LogViewState *state) {
	DB_GetLogTable(app->db, &state->table_data.working_table);

	state->table_data.msg_no_entries = "  No diary entries under this category.";
	state->table_data.col_width_values = cols_width;
}

static void LogView_HandleInputNav(App *app, LogViewState *state) {
	TableWidgetData_HandleInput(
			&state->table_data,
			app->ch
			);

	if(app->ch == 'd' && state->table_data.working_table.nrow > 1) {
		state->input_state = LOGVIEW_INPUT_DELETE;
	}

	if((app->ch == 'c' || app->ch == 'r') && state->table_data.working_table.nrow > 1) {
		app->next_state = STATE_UPDATELOG;

		AddLog_SetUpdateData(
				app,
				TableWidgetData_GetSelectedRow(&state->table_data)
				);
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

static void LogView_HandleInput(App *app, LogViewState *state) {
	switch(state->input_state) {
		case LOGVIEW_INPUT_NAV:
			LogView_HandleInputNav(app, state);
			break;

		case LOGVIEW_INPUT_DELETE:
			LogView_HandleInputDelete(app, state);
			break;
	}
}

static void LogView_DrawTable(App *app, LogViewState *state) {
	WINDOW *win = app->content_window.win;

	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	if(state->input_state == LOGVIEW_INPUT_DELETE) {
		LogView_DrawDeleteConfirmation(app, state);
	} else {
		App_ClearBottomWindow(app);
	}
}

static void LogView_DrawDeleteConfirmation(App *app, LogViewState *state) {
	WINDOW *win = app->bottom_window.win;
	(void) state;
	
	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);
	Widget_Label(win, " Delete diary entry? Y/n", -1);
	Widget_Space(win, -1);
}

void LogView_Update(App *app) {
	LogViewState *state = (LogViewState *) app->states[STATE_LOGVIEW];

	if(App_IsNewState(app)) {
		LogView_UpdateTable(app, state);

		app->content_window.owner_state = STATE_LOGVIEW;
		state->input_state = LOGVIEW_INPUT_NAV;
	}

	LogView_HandleInput(app, state);
	LogView_DrawTable(app, state);
}
