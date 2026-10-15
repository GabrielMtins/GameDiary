#include "App.h"

static const char *labels[] = {
	" Start date:",
	" End date:",
	" Score: ",
};

static void AddLog_SetUpForm(App *app, AddLogState *state);

static void AddLog_SetUpForm(App *app, AddLogState *state) {
	state->form_data.num_inputs = 3;

	state->form_data.form_title = state->form_title_data;
	state->form_data.labels = labels;
	state->form_data.labels_width = 13;

	state->form_data.callback_args = (void *) app;
	state->form_data.cancel_callback = NULL;
	state->form_data.ok_callback = NULL;
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
