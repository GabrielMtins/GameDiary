#include "App.h"
#include "SqlCmds.h"

static const char *labels[] = {
	" Start date:",
	" End date:",
	" Score: ",
};

static void AddLog_SetUpForm(App *app, AddLogState *state);
static void AddLog_HandleAddQuery(App *app, AddLogState *state);
static void AddLog_OkCallback(void *args);
static void AddLog_CancelCallback(void *args);

static void AddLog_SetUpForm(App *app, AddLogState *state) {
	state->form_data.num_inputs = 3;

	state->form_data.form_title = state->form_title_data;
	state->form_data.labels = labels;
	state->form_data.labels_width = 13;

	state->form_data.callback_args = (void *) app;
	state->form_data.cancel_callback = AddLog_CancelCallback;
	state->form_data.ok_callback = AddLog_OkCallback;
}

static void AddLog_HandleAddQuery(App *app, AddLogState *state) {
	char *cmd = NULL;
	int len;

	for(int i = 0; i < state->form_data.num_inputs; i++) {
		LineWidgetData *input = &state->form_data.inputs[i];

		input->str[input->len] = '\0';
	}

	len = snprintf(cmd, 0, sql_cmd_insert_log,
			state->chosen_game,
			state->form_data.inputs[0].str,
			state->form_data.inputs[1].str,
			state->form_data.inputs[2].str
			) + 1;

	cmd = (char *) malloc(len);

	snprintf(cmd, len, sql_cmd_insert_log,
			state->chosen_game,
			state->form_data.inputs[0].str,
			state->form_data.inputs[1].str,
			state->form_data.inputs[2].str
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

static void AddLog_OkCallback(void *args) {
	App *app = (App *) args;

	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	AddLog_HandleAddQuery(app, state);

	app->next_state = STATE_CATALOG;
}

static void AddLog_CancelCallback(void *args) {
	App *app = (App *) args;

	app->next_state = STATE_CATALOG;
}

void AddLog_SetGame(App *app, const char *str) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	strncpy(state->chosen_game, str, LOG_STR_SIZE - 1);

	snprintf(
			state->form_title_data,
			LOG_STR_SIZE,
			"  Log game: %s",
			state->chosen_game
			);
}

void AddLog_Update(App *app) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	if(state->form_data.num_inputs == 0) {
		AddLog_SetUpForm(app, state);
	}

	if(App_IsNewState(app)) {
		state->form_data.should_clear = true;
		app->content_window.owner_state = STATE_ADDLOG;
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
