#include "App.h"

#include <ctype.h>

enum AddGameInputEnum {
	ADDGAME_INPUT_NAME = 0,
	ADDGAME_INPUT_GENRE,
	ADDGAME_INPUT_PLATFORM,
	ADDGAME_INPUT_YEAR
};

static const char *labels[] = {
	" Name:",
	" Genre:",
	" Platform:",
	" Year:"
};

#define ADD_GAME_NUM_INPUTS ((int) (sizeof(labels) / sizeof(const char *)))

static void AddGame_CancelCallback(void *args);
static void AddGame_OkCallback(void *args);
static bool AddGame_VerifyInputs(App *app, AddGameState *state);
static void AddGame_ShowErrorMsg(App *app, AddGameState *state);

static void AddGame_CancelCallback(void *args) {
	App *app = (App *) args;

	app->next_state = STATE_CATALOG;
}

static void AddGame_OkCallback(void *args) {
	App *app = (App *) args;
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];
	LineWidgetData *inputs = state->form_data.inputs;

	FormWidgetData_AddTerminatorToInputs(&state->form_data);

	if(!AddGame_VerifyInputs(app, state)) {
		return;
	}

	switch(app->current_state) {
		case STATE_UPDATEGAME:
			DB_UpdateGame(
					app->db,
					inputs[ADDGAME_INPUT_NAME].str,
					inputs[ADDGAME_INPUT_GENRE].str,
					inputs[ADDGAME_INPUT_PLATFORM].str,
					atoi(inputs[ADDGAME_INPUT_YEAR].str)
					);
			break;

		case STATE_ADDGAME:
			DB_InsertGame(
					app->db,
					inputs[ADDGAME_INPUT_NAME].str,
					inputs[ADDGAME_INPUT_GENRE].str,
					inputs[ADDGAME_INPUT_PLATFORM].str,
					atoi(inputs[ADDGAME_INPUT_YEAR].str)
					);
			break;
	}

	app->next_state = STATE_CATALOG;
}

static bool AddGame_VerifyInputs(App *app, AddGameState *state) {
	(void) app;

	const char *str;
	int len;

	if(state->form_data.inputs[ADDGAME_INPUT_NAME].len == 0) {
		state->error_msg = "Error: Game name must not be empty.";
		return false;
	}

	str = state->form_data.inputs[ADDGAME_INPUT_YEAR].str;
	len = state->form_data.inputs[ADDGAME_INPUT_YEAR].len;

	for(int i = 0; i < len; i++) {
		if(!isdigit(str[i])) {
			state->error_msg = "Error: Year must be a number.";
			return false;
		}
	}

	return true;
}

static void AddGame_ShowErrorMsg(App *app, AddGameState *state) {
	if(state->error_msg == NULL) return;

	WINDOW *win = app->bottom_window;

	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);

	Widget_Space(win, 1);
	Widget_Label(win, state->error_msg, -1);
	Widget_Space(win, -1);
}

void AddGame_Init(App *app) {
	AddGameState *state = app->states[STATE_ADDGAME];

	if(state->form_data.num_inputs == 0) {
		FormWidgetData_Init(&state->form_data);

		state->form_data.num_inputs = ADD_GAME_NUM_INPUTS;
	
		state->form_data.form_title = "  Write game information.";
		state->form_data.labels = labels;
		state->form_data.labels_width = 11;
	
		state->form_data.callback_args = (void *) app;
		state->form_data.cancel_callback = AddGame_CancelCallback;
		state->form_data.ok_callback = AddGame_OkCallback;
	}

	state->error_msg = NULL;
	state->form_data.should_clear = true;

	if(app->current_state == STATE_ADDGAME) {
		FormWidgetData_ResetInput(&state->form_data);
	}
}

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	Widget_Form(
			app->content_window,
			&state->form_data,
			app->ch
			);

	AddGame_ShowErrorMsg(app, state);
}

void AddGame_SetUpdateData(App *app, char **data) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		LineWidgetData *line_data = &state->form_data.inputs[i];

		strncpy(
				line_data->str,
				data[i],
				MAX_LINE_SIZE
				);

		line_data->len = strlen(line_data->str);
		line_data->cursor_pos = line_data->len + 1;
	}

	strncpy(state->update_game, data[ADDGAME_INPUT_NAME], MAX_LINE_SIZE);

	state->form_data.input_selected = 0;
}
