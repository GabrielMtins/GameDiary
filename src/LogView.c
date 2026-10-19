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

void LogView_Init(App *app) {
	TableManagerState *state = app->states[STATE_LOGVIEW];

	state->table_data.msg_no_entries = "  There are no games under this category.";
	state->table_data.col_width_values = cols_width;

	state->update_table_callback = LogView_UpdateTable;
	state->delete_item_callback = LogView_DeleteItem;

	TableManager_Init(app, state);
}

void LogView_Update(App *app) {
	TableManager_Update(app, app->states[STATE_LOGVIEW]);
}
