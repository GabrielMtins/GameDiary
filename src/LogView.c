#include "App.h"

static const int cols_width[] = {
	0,
	37,
	16,
	16,
	11,
};

static void LogView_UpdateTable(App *app, TableManagerState *state);
static void LogView_DeleteItem(App *app, TableManagerState *state);
static void LogView_HandleInputNav(App *app, TableManagerState *state);

static void LogView_UpdateTable(App *app, TableManagerState *state) {
	DB_SearchLog(
			app->db,
			&state->table_data.working_table,
			state->search_data.str
			);
}

static void LogView_DeleteItem(App *app, TableManagerState *state) {
	DB_DeleteLog(
			app->db,
			TableWidgetData_GetSelectedRow(&state->table_data)[0]
			);
}

static void LogView_HandleInputNav(App *app, TableManagerState *state) {
	bool is_table_valid = state->table_data.working_table.nrow > 1;

	switch(app->ch) {
		case 'c':
		case 'r':
			if(!is_table_valid) break;

			app->next_state = STATE_UPDATELOG;

			AddLog_SetUpdateData(
					app,
					TableWidgetData_GetSelectedRow(&state->table_data)
					);

			break;
	}
}

void LogView_Init(App *app) {
	TableManagerState *state = app->states[STATE_LOGVIEW];

	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;

	state->update_table_callback = LogView_UpdateTable;
	state->delete_item_callback = LogView_DeleteItem;
	state->input_nav_callback = LogView_HandleInputNav;

	TableManager_Init(app, state);
}

void LogView_Update(App *app) {
	TableManager_Update(app, app->states[STATE_LOGVIEW]);
}
