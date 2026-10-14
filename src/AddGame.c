#include "App.h"

#include <string.h>
#include <ctype.h>

#define GAME_LABEL_START_X 2
#define GAME_LABEL_START_Y 2
#define GAME_LABEL_SIZE 11
#define GAME_LABEL_END_X (GAME_LABEL_START_X + GAME_LABEL_SIZE)

#define game_label_start(i) GAME_LABEL_START_Y + i * 2, GAME_LABEL_START_X
#define game_label_end(i)   GAME_LABEL_START_Y + i * 2, GAME_LABEL_END_X

static const char *labels[] = {
	" Name:",
	" Genre:",
	" Platform:",
	" Year:"
};

static void AddGame_Start(App *app, AddGameState *state);
static void AddGame_DrawClear(App *app);
static void AddGame_HandleInput(App *app, AddGameState *state);

static void AddGame_Start(App *app, AddGameState *state) {
	AddGame_DrawClear(app);

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		state->inputs[i].cursor_pos = 0;
		state->inputs[i].len = 0;
		state->inputs[i].width = app->content_window.width - GAME_LABEL_END_X - 2;
		state->inputs[i].bg_attron = A_REVERSE | A_DIM;
		state->inputs[i].cursor_attron = A_REVERSE | A_BOLD;
	}
}

static void AddGame_DrawClear(App *app) {
	WINDOW *win = app->content_window.win;

	//wbkgdset(win, ' ' | A_REVERSE | A_DIM);
	wclear(win);
	//wbkgdset(win, ' ');

	wattrset(win, A_REVERSE);

	{
		wmove(win, 0, 0);
		Widget_Space(win, -1);
		wmove(win, 0, 2);
		Widget_Label(win, "Add Game", -1);
	}

	wattrset(win, A_REVERSE);

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		wmove(
				win,
				game_label_start(i)
			 );

		Widget_Label(win, labels[i], GAME_LABEL_SIZE);
	}
}

static void AddGame_HandleInput(App *app, AddGameState *state) {
	if(app->ch == KEY_DOWN || app->ch == '\n' || app->ch == '\t') {
		state->input_selected++;
	}

	if(app->ch == KEY_UP || app->ch == KEY_BTAB) {
		state->input_selected--;
	}

	if(state->input_selected < 0) {
		state->input_selected = 0;
	}

	if(state->input_selected >= ADD_GAME_NUM_INPUTS) {
		state->input_selected = ADD_GAME_NUM_INPUTS - 1;
	}

	LineWidgetData_HandleInput(
			&state->inputs[state->input_selected],
			app->ch
			);
}

static void AddGame_DrawInput(App *app, AddGameState *state) {
	WINDOW *win = app->content_window.win;

	wattrset(win, A_REVERSE | A_DIM);

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		wmove(
				win,
				game_label_end(i)
			 );

		if(i == state->input_selected) {
			state->inputs[i].cursor_attron = A_BOLD | A_REVERSE;
		} else {
			state->inputs[i].cursor_attron = state->inputs[i].bg_attron;
		}

		Widget_InputLine(win, &state->inputs[i]);
	}
}

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(app->prev_state != STATE_ADDGAME) {
		AddGame_Start(app, state);
	}

	AddGame_HandleInput(app, state);
	AddGame_DrawInput(app, state);
}
