#include "App.h"
#include "SqlCmds.h"

static const char *labels[] = {
	" Name:",
	" Genre:",
	" Platform:",
	" Year:"
};

static void AddGame_SetUpForm(App *app, AddGameState *state);
static void AddGame_CancelCallback(void *args);
static void AddGame_OkCallback(void *args);
static void AddGame_HandleAddQuery(App *app, AddGameState *state);

static void AddGame_SetUpForm(App *app, AddGameState *state) {
	state->form_data.num_inputs = 4;

	state->form_data.form_title = "  Add game";
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

	AddGame_HandleAddQuery(app, state);

	app->next_state = STATE_CATALOG;
}

static void AddGame_HandleAddQuery(App *app, AddGameState *state) {
	char *cmd = NULL;
	int len;

	for(int i = 0; i < state->form_data.num_inputs; i++) {
		LineWidgetData *input = &state->form_data.inputs[i];

		input->str[input->len] = '\0';
	}

	len = snprintf(cmd, 0, sql_cmd_insert_game,
			state->form_data.inputs[0].str,
			state->form_data.inputs[1].str,
			state->form_data.inputs[2].str,
			state->form_data.inputs[3].str
			) + 1;

	cmd = (char *) malloc(len);

	snprintf(cmd, len, sql_cmd_insert_game,
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

void AddGame_Update(App *app) {
	AddGameState *state = (AddGameState *) app->states[STATE_ADDGAME];

	if(state->form_data.num_inputs == 0) {
		AddGame_SetUpForm(app, state);
	}

	if(App_IsNewState(app)) {
		state->form_data.should_clear = true;
		FormWidgetData_ResetInput(&state->form_data);
	}

	if(app->ch == KEY_ESCAPE) {
		app->next_state = STATE_CATALOG;
	}

	Widget_Form(
			app->content_window.win,
			&state->form_data,
			app->ch
			);
}
