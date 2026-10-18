#include "App.h"
#include <ctype.h>

enum AddLogInputEnum {
	ADDLOG_INPUT_STARTDATE = 0,
	ADDLOG_INPUT_ENDDATE,
	ADDLOG_INPUT_SCORE,
	ADDLOG_NUM_INPUTS,
};

enum AddLogUpdateArgsEnum {
	ADDLOG_UPDATE_ID,
	ADDLOG_UPDATE_GAMENAME,
	ADDLOG_UPDATE_STARTDATE,
	ADDLOG_UPDATE_ENDDATE,
	ADDLOG_UPDATE_SCORE,
};

static const char *labels[] = {
	" Start date:",
	" End date:",
	" Score: ",
};

static bool AddLog_VerifyIfInputIsDate(const LineWidgetData *data);
static bool AddLog_VerifyInputs(App *app, AddLogState *state);
static void AddLog_BackToPrevState(App *app);
static void AddLog_OkCallback(void *args);
static void AddLog_CancelCallback(void *args);
static void AddLog_ShowErrorMsg(App *app, AddLogState *state);

static void AddLog_BackToPrevState(App *app) {
	switch(app->current_state) {
		case STATE_ADDLOG:
			app->next_state = STATE_CATALOG;
			break;

		case STATE_UPDATELOG:
			app->next_state = STATE_LOGVIEW;
			break;
	}
}

static void AddLog_OkCallback(void *args) {
	App *app = (App *) args;

	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];
	LineWidgetData *inputs = state->form_data.inputs;

	FormWidgetData_AddTerminatorToInputs(&state->form_data);

	if(!AddLog_VerifyInputs(app, state)) {
		return;
	}

	switch(app->current_state) {
		case STATE_ADDLOG:
			DB_InsertLog(
					app->db,
					state->chosen_game,
					inputs[ADDLOG_INPUT_STARTDATE].str,
					inputs[ADDLOG_INPUT_ENDDATE].str,
					atoi(inputs[ADDLOG_INPUT_SCORE].str)
					);
			break;

		case STATE_UPDATELOG:
			DB_UpdateLog(
					app->db,
					inputs[ADDLOG_INPUT_STARTDATE].str,
					inputs[ADDLOG_INPUT_ENDDATE].str,
					atoi(inputs[ADDLOG_INPUT_SCORE].str),
					state->chosen_game
					);
			break;
	}

	AddLog_BackToPrevState(app);
}

static bool AddLog_VerifyIfInputIsDate(const LineWidgetData *data) {
	if(data->len == 0) {
		return true;
	}

	if(data->len != 10) {
		return false;
	}

	if(data->str[2] != '/') return false;
	if(data->str[5] != '/') return false;

	for(int i = 0; i < data->len; i++) {
		if(i == 2 || i == 5) continue;

		if(!isdigit(data->str[i])) {
			return false;
		}
	}

	return true;
}

static bool AddLog_VerifyInputs(App *app, AddLogState *state) {
	(void) app;

	char *str;
	int len;

	len = state->form_data.inputs[ADDLOG_INPUT_STARTDATE].len;

	if(len == 0) {
		state->error_msg = "Error: Start date must not be empty.";
		return false;
	}

	state->error_msg = "Error: Date must be on the format xx/xx/xxxx.";

	if(!AddLog_VerifyIfInputIsDate(&state->form_data.inputs[ADDLOG_INPUT_STARTDATE])) {
		return false;
	}

	if(!AddLog_VerifyIfInputIsDate(&state->form_data.inputs[ADDLOG_INPUT_ENDDATE])) {
		return false;
	}

	str = state->form_data.inputs[ADDLOG_INPUT_SCORE].str;
	len = state->form_data.inputs[ADDLOG_INPUT_SCORE].len;

	for(int i = 0; i < len; i++) {
		if(!isdigit(str[i])) {
			state->error_msg = "Error: Score must be a number.";
			return false;
		}
	}

	state->error_msg = NULL;

	return true;
}

static void AddLog_CancelCallback(void *args) {
	App *app = (App *) args;

	AddLog_BackToPrevState(app);
}

static void AddLog_ShowErrorMsg(App *app, AddLogState *state) {
	if(state->error_msg == NULL) return;

	WINDOW *win = app->bottom_window;

	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);

	Widget_Space(win, 1);
	Widget_Label(win, state->error_msg, -1);
	Widget_Space(win, -1);
}

void AddLog_SetUpdateData(App *app, char **data) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	AddLog_SetGame(app, data[ADDLOG_UPDATE_GAMENAME]);

	for(int i = 0; i < ADDLOG_NUM_INPUTS; i++) {
		char *str = state->form_data.inputs[i].str;

		strncpy(
				str,
				data[ADDLOG_UPDATE_STARTDATE + i],
				MAX_LINE_SIZE 
			   );

		int len = strlen(str);

		state->form_data.inputs[i].len = len;
		state->form_data.inputs[i].cursor_pos = len + 1;
	}

	strncpy(state->chosen_game, data[ADDLOG_UPDATE_ID], MAX_LINE_SIZE);

	state->form_data.input_selected = 0;
}

void AddLog_SetGame(App *app, const char *str) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	strncpy(state->chosen_game, str, MAX_LINE_SIZE);

	snprintf(
			state->form_title_data,
			MAX_LINE_SIZE,
			"  Log game: %s",
			state->chosen_game
			);
}

void AddLog_Init(App *app) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	if(state->form_data.num_inputs == 0) {
		FormWidgetData_Init(&state->form_data);

		state->form_data.num_inputs = 3;
	
		state->form_data.form_title = state->form_title_data;
		state->form_data.labels = labels;
		state->form_data.labels_width = 13;
	
		state->form_data.callback_args = (void *) app;
		state->form_data.cancel_callback = AddLog_CancelCallback;
		state->form_data.ok_callback = AddLog_OkCallback;
	}

	state->error_msg = NULL;

	if(app->current_state == STATE_ADDLOG) {
		FormWidgetData_ResetInput(&state->form_data);
	}
}

void AddLog_Update(App *app) {
	AddLogState *state = (AddLogState *) app->states[STATE_ADDLOG];

	Widget_Form(
			app->content_window,
			&state->form_data,
			app->ch
			);

	AddLog_ShowErrorMsg(app, state);
}
