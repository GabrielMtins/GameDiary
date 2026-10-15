#include "App.h"
#include "SqlCmds.h"

static void LogView_UpdateTable(App *app, LogViewState *state);
static void LogView_HandleInput(App *app, LogViewState *state);
static void LogView_DrawTable(App *app, LogViewState *state);

static void LogView_UpdateTable(App *app, LogViewState *state) {
	Table_GetSqlTable(
			&state->table_data.working_table,
			app->db,
			sql_cmd_get_log_table
			);

	state->table_data.msg_no_entries = "  No diary entries under this category.";
	state->table_data.start_row = 1;
}

static void LogView_HandleInput(App *app, LogViewState *state) {
	TableWidgetData_HandleInput(
			&state->table_data,
			app->ch
			);

	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_MENUBAR;
	}
}

static void LogView_DrawTable(App *app, LogViewState *state) {
	WINDOW *win = app->content_window.win;

	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);

	App_ClearBottomWindow(app);
}

void LogView_Update(App *app) {
	LogViewState *state = (LogViewState *) app->states[STATE_LOGVIEW];

	if(App_IsNewState(app)) {
		LogView_UpdateTable(app, state);

		app->content_window.owner_state = STATE_LOGVIEW;
	}

	LogView_HandleInput(app, state);
	LogView_DrawTable(app, state);
}
