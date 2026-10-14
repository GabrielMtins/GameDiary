#include "App.h"
#include "cmds.h"

static void Catalog_DrawClear(App *app, CatalogState *state);
static void Catalog_DrawTable(App *app, CatalogState *state);
static void Catalog_HandleInput(App *app, CatalogState *state);

static void Catalog_DrawClear(App *app, CatalogState *state) {
	WINDOW *win = app->content_window.win;

	wclear(win);

	wattrset(win, A_REVERSE);

	Table_PrintRowOnWindow(
			&app->content_window,
			&state->working_table,
			0,
			0
			);
}

static void Catalog_DrawTable(App *app, CatalogState *state) {
	WINDOW *win = app->content_window.win;

	state->start_row = 1;

	for(int line = 1; line < app->content_window.height; line++) {
		int current_row = (line - 1) + state->start_row;

		if(current_row >= state->working_table.nrow) {
			break;
		}

		if(current_row == state->row_selected) {
			wattrset(win, A_REVERSE | A_BOLD | A_DIM);
		} else {
			wattrset(win, A_NORMAL);
		}

		Table_PrintRowOnWindow(
				&app->content_window,
				&state->working_table,
				line,
				current_row
				);
	}
}

static void Catalog_HandleInput(App *app, CatalogState *state) {
	if(app->ch == 'j') {
		state->row_selected++;
	}

	if(app->ch == 'k') {
		state->row_selected--;
	}

	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_MENUBAR;
	}

	if(state->row_selected < 1) {
		state->row_selected = 1;
	}

	if(state->row_selected >= state->working_table.nrow) {
		state->row_selected = state->working_table.nrow - 1;
	}
}

void Catalog_Update(App *app) {
	CatalogState *state = app->states[STATE_CATALOG];

	if(app->prev_state != STATE_CATALOG || app->ch == KEY_RESIZE) {
		Table_GetSqlTable(
				&state->working_table,
				app->db,
				get_game_table_cmd
				);
		
		Catalog_DrawClear(app, state);

		app->content_window.owner_state = STATE_CATALOG;

		state->row_selected = 1;
	}
	
	Catalog_HandleInput(app, state);

	Catalog_DrawTable(app, state);
}
