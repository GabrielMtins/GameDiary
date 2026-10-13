#include "App.h"

static void Catalog_PrintTable(App *app);

static void Catalog_PrintTable(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	wattrset(app->content_window.win, A_REVERSE | A_DIM);
	Table_PrintRowOnWindow(
			&app->content_window,
			&state->working_table,
			0,
			0
			);
	state->start_row = 1;

	for(int i = state->start_row; i < state->working_table.row + 1; i++) {
		if(i == state->row_selected) {
			wattrset(app->content_window.win, A_REVERSE);
		} else{
			wattrset(app->content_window.win, A_NORMAL);
		}
		Table_PrintRowOnWindow(
				&app->content_window,
				&state->working_table,
				i - state->start_row + 1,
				i
				);
	}

}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	if(app->ch == 'j') {
		state->row_selected++;
	}

	if(app->ch == 'k') {
		state->row_selected--;
	}

	if(app->ch == 27) {
		app->current_state = STATE_MENUBAR;
	}

	Catalog_PrintTable(app);
}
