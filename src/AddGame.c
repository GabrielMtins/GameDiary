#include "App.h"
#include "SqlCmds.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define GAME_LABEL_START_X 2
#define GAME_LABEL_START_Y 2
#define GAME_LABEL_SIZE 11
#define GAME_LABEL_END_X (GAME_LABEL_START_X + GAME_LABEL_SIZE)

#define ADD_GAME_NUM_BUTTONS 2

#define ADD_GAME_BUTTON_CANCEL (ADD_GAME_NUM_INPUTS + 0)
#define ADD_GAME_BUTTON_OK (ADD_GAME_NUM_INPUTS + 1)

#define game_label_start(i) GAME_LABEL_START_Y + i * 2, GAME_LABEL_START_X
#define game_label_end(i)   GAME_LABEL_START_Y + i * 2, GAME_LABEL_END_X

static const char *labels[] = {
	" Name:",
	" Genre:",
	" Platform:",
	" Year:"
};

static void AddGame_ResetInput(App *app, AddGameState *state);
static void AddGame_DrawClear(App *app);
static void AddGame_HandleInput(App *app, AddGameState *state);
static void AddGame_DrawInput(App *app, AddGameState *state);
static void AddGame_DrawButton(App *app, AddGameState *state);
static void AddGame_HandleAddQuery(App *app, AddGameState *state);

static void AddGame_ResetInput(App *app, AddGameState *state) {
	AddGame_DrawClear(app);

	state->input_selected = 0;

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		state->inputs[i].width = app->content_window.width - GAME_LABEL_END_X - 2;
		state->inputs[i].len = 0;
		state->inputs[i].cursor_pos = 0;
		state->inputs[i].bg_attron = A_DIM | A_REVERSE;
	}
}

static void AddGame_DrawClear(App *app) {
	WINDOW *win = app->content_window.win;

	wclear(win);

	wattrset(win, A_REVERSE);

	{
		wmove(win, 0, 0);
		Widget_Label(win, "  Add game", -1);
		Widget_Space(win, -1);
	}

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		wmove(
				win,
				game_label_start(i)
			 );

		Widget_Label(win, labels[i], GAME_LABEL_SIZE);
	}
}

static void AddGame_HandleInput(App *app, AddGameState *state) {
	if(app->ch == '\n' && state->input_selected == ADD_GAME_BUTTON_CANCEL) {
		app->next_state = STATE_CATALOG;
	}

	if(app->ch == '\n' && state->input_selected == ADD_GAME_BUTTON_OK) {
		AddGame_HandleAddQuery(app, state);
		app->next_state = STATE_CATALOG;
	}

	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_CATALOG;
	}

	if(app->ch == KEY_DOWN || app->ch == '\n' || app->ch == '\t') {
		state->input_selected++;
	}

	if(app->ch == KEY_UP || app->ch == KEY_BTAB) {
		state->input_selected--;
	}

	LIMIT_VAR(state->input_selected, 0, ADD_GAME_NUM_INPUTS + ADD_GAME_NUM_BUTTONS);

	if(state->input_selected < ADD_GAME_NUM_INPUTS) {
		LineWidgetData_HandleInput(
				&state->inputs[state->input_selected],
				app->ch
				);
	}
}

static void AddGame_DrawInput(App *app, AddGameState *state) {
	WINDOW *win = app->content_window.win;

	wattrset(win, A_REVERSE | A_DIM);

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		wmove(
				win,
				game_label_end(i)
			 );

		state->inputs[i].width = app->content_window.width - GAME_LABEL_END_X - 2;

		if(i == state->input_selected) {
			state->inputs[i].cursor_attron = A_BOLD | A_REVERSE;
		} else {
			state->inputs[i].cursor_attron = state->inputs[i].bg_attron;
		}

		Widget_InputLine(win, &state->inputs[i]);
	}
}

static void AddGame_DrawButton(App *app, AddGameState *state) {
	WINDOW *win = app->content_window.win;

	static const int button_padding = 5;
	static const int button_width = 10;
	const int middle_screen = app->content_window.width / 2;

	wmove(win, 10, middle_screen - button_padding - button_width);

	Widget_Button(
			win,
			"  Cancel  ",
			-1,
			state->input_selected == (ADD_GAME_NUM_INPUTS)
			);

	wmove(win, 10, middle_screen + button_padding);

	Widget_Button(
			win,
			"    OK    ",
			-1,
			state->input_selected == (ADD_GAME_NUM_INPUTS + 1)
			);
}

static void AddGame_HandleAddQuery(App *app, AddGameState *state) {
	char *cmd = NULL;
	int len;

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		LineWidgetData *input = &state->inputs[i];

		input->str[input->len] = '\0';
	}

	len = snprintf(cmd, 0, sql_cmd_insert_game,
			state->inputs[0].str,
			state->inputs[1].str,
			state->inputs[2].str,
			state->inputs[3].str
			) + 1;

	cmd = (char *) malloc(len);

	snprintf(cmd, len, sql_cmd_insert_game,
			state->inputs[0].str,
			state->inputs[1].str,
			state->inputs[2].str,
			state->inputs[3].str
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

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(app->ch == KEY_RESIZE) {
		AddGame_DrawClear(app);
	}

	if(app->prev_state != STATE_ADDGAME) {
		AddGame_ResetInput(app, state);

		app->content_window.owner_state = STATE_ADDGAME;
	}

	AddGame_HandleInput(app, state);

	AddGame_DrawInput(app, state);
	AddGame_DrawButton(app, state);
}
