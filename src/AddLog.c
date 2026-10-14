#include "App.h"

#define LOG_LABEL_START_X 2
#define LOG_LABEL_START_Y 2
#define LOG_LABEL_SIZE 11
#define LOG_LABEL_END_X (LOG_LABEL_START_X + LOG_LABEL_SIZE)

#define ADD_LOG_NUM_BUTTONS 2

#define ADD_LOG_BUTTON_CANCEL (ADD_LOG_NUM_INPUTS + 0)
#define ADD_LOG_BUTTON_OK (ADD_LOG_NUM_INPUTS + 1)

#define game_label_start(i) LOG_LABEL_START_Y + i * 2, LOG_LABEL_START_X
#define game_label_end(i)   LOG_LABEL_START_Y + i * 2, LOG_LABEL_END_X

static void AddLog_ResetInput(App *app, AddLogState *state);
static void AddLog_DrawClear(App *app);

static void AddLog_ResetInput(App *app, AddLogState *state) {
	//AddGame_DrawClear(app);

	state->input_selected = 0;

	for(int i = 0; i < ADD_GAME_NUM_INPUTS; i++) {
		state->inputs[i].width = app->content_window.width - LOG_LABEL_END_X - 2;
		state->inputs[i].len = 0;
		state->inputs[i].cursor_pos = 0;
		state->inputs[i].bg_attron = A_DIM | A_REVERSE;
	}
}

void AddLog_Update(App *app) {

}
