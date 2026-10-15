#include "App.h"
#include "SqlCmds.h"
#include "Util.h"

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
static void LogView_HandleDeleteQuery(App *app, LogViewState *state);
static void LogView_DrawTable(App *app, LogViewState *state);
static void LogView_DrawDeleteConfirmation(App *app, LogViewState *state);

static void LogView_UpdateTable(App *app, LogViewState *state) {
	Table_GetSqlTable(
			&state->table_data.working_table,
			app->db,
			sql_cmd_get_log_table
			);

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
}

static void LogView_HandleInputDelete(App *app, LogViewState *state) {
	if(app->ch == 'n' || app->ch == KEY_ESCAPE) {
		state->input_state = LOGVIEW_INPUT_NAV;
	}

	if(app->ch == '\n' || app->ch == 'y') {
		state->input_state = LOGVIEW_INPUT_NAV;

		LogView_HandleDeleteQuery(app, state);
		LogView_UpdateTable(app, state);
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

static void LogView_HandleDeleteQuery(App *app, LogViewState *state) {
	char *cmd;
	Table *table = &state->table_data.working_table;

	const char *selected_id = table->str[table->ncol * state->table_data.selected_row];

	cmd = safe_sprintf(
			sql_cmd_delete_log,
			selected_id
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
