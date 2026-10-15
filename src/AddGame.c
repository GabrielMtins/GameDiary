#include "App.h"
#include "SqlCmds.h"
#include "Util.h"

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

static void AddGame_SetUpForm(App *app, AddGameState *state);
static void AddGame_CancelCallback(void *args);
static void AddGame_OkCallback(void *args);
static bool AddGame_VerifyInputs(App *app, AddGameState *state);
static void AddGame_HandleAddQuery(App *app, AddGameState *state);
static void AddGame_ShowErrorMsg(App *app, AddGameState *state);

static void AddGame_SetUpForm(App *app, AddGameState *state) {
	state->form_data.num_inputs = 4;

	state->form_data.form_title = "  Write game information.";
	state->form_data.labels = labels;
	state->form_data.labels_width = 11;

	state->form_data.callback_args = (void *) app;
	state->form_data.cancel_callback = AddGame_CancelCallback;
	state->form_data.ok_callback = AddGame_OkCallback;
}

static void AddGame_CancelCallback(void *args) {
	App *app = (App *) args;

	app->next_state = STATE_CATALOG;
}

static void AddGame_OkCallback(void *args) {
	App *app = (App *) args;
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(!AddGame_VerifyInputs(app, state)) {
		return;
	}

	AddGame_HandleAddQuery(app, state);

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

static void AddGame_HandleAddQuery(App *app, AddGameState *state) {
	char *cmd;

	FormWidgetData_AddTerminatorToInputs(&state->form_data);

	cmd = safe_sprintf(
			sql_cmd_insert_game,
			state->form_data.inputs[0].str,
			state->form_data.inputs[1].str,
			state->form_data.inputs[2].str,
			state->form_data.inputs[3].str
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

static void AddGame_ShowErrorMsg(App *app, AddGameState *state) {
	if(state->error_msg == NULL) return;

	WINDOW *win = app->bottom_window.win;

	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);

	Widget_Space(win, 1);
	Widget_Label(win, state->error_msg, -1);
	Widget_Space(win, -1);
}

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(state->form_data.num_inputs == 0) {
		AddGame_SetUpForm(app, state);
	}

	if(App_IsNewState(app)) {
		state->form_data.should_clear = true;
		state->error_msg = NULL;
		FormWidgetData_ResetInput(&state->form_data);
	}

	Widget_Form(
			app->content_window.win,
			&state->form_data,
			app->ch
			);

	AddGame_ShowErrorMsg(app, state);
}
