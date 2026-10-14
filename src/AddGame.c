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

		state->inputs[i].width = app->content_window.width - GAME_LABEL_END_X - 2;

		Widget_InputLine(win, &state->inputs[i]);
	}
}

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(app->prev_state != STATE_ADDGAME) {
		AddGame_DrawClear(app);
	}

	AddGame_HandleInput(app, state);
	AddGame_DrawInput(app, state);
}
