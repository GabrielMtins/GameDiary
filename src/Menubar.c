#include "App.h"
#include "Widgets.h"

#define NUM_BUTTONS ((int) (sizeof(button_labels) / sizeof(const char *)))

static const char *button_labels[] = {
	" Games",
	" Diary",
	" Stats",
	" About"
};

static const int button_to_state[] = {
	STATE_CATALOG,
	STATE_LOGVIEW,
	STATE_NONE,
	STATE_NONE,
};

static bool Menubar_CheckIfStateIsOfMenubar(App *app);
static void Menubar_HandleInput(App *app, MenubarState *state);
static void Menubar_Draw(App *app, MenubarState *state);

static bool Menubar_CheckIfStateIsOfMenubar(App *app) {
	for(int i = 0; i < NUM_BUTTONS; i++) {
		if(app->current_state == button_to_state[i]) {
			return true;
		}
	}

	return false;
}

static void Menubar_HandleInput(App *app, MenubarState *state) {
	if(!Menubar_CheckIfStateIsOfMenubar(app)) {
		return;
	}

	if(app->ch == '\t' || app->ch == 'K' || (app->prev_ch == 'g' && app->ch == 't')) {
		state->button_selected++;
	}

	if(app->ch == KEY_BTAB || app->ch == 'J' || (app->prev_ch == 'g' && app->ch == 'T')) {
		state->button_selected--;
	}

	if(app->ch == 'q') {
		app->quit = 1;
	}

	if(state->button_selected < 0) state->button_selected = NUM_BUTTONS - 1;
	if(state->button_selected >= NUM_BUTTONS) state->button_selected = 0;

	app->next_state = button_to_state[state->button_selected];
}

static void Menubar_Draw(App *app, MenubarState *state) {
	WINDOW *win = app->top_window.win;

	wmove(win, 0, 0);
	wattrset(win, A_NORMAL);
	Widget_Space(win, 2);

	for(int i = 0; i < NUM_BUTTONS; i++) {
		Widget_Button(
				win,
				button_labels[i],
				10,
				state->button_selected == i
				);

		wattrset(win, A_NORMAL);
		Widget_Space(win, 2);
	}
}

void Menubar_Update(App *app) {
	MenubarState *state = (MenubarState *) app->states[STATE_MENUBAR];

	Menubar_HandleInput(app, state);
	Menubar_Draw(app, state);
}
