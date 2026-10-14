#include "App.h"
#include "Draw.h"
#include "Widgets.h"

#define NUM_BUTTONS 3

static const char *button_labels[] = {
	" Games",
	" Diary",
	" Stats",
};

static void Menubar_Draw(App *app) {
	MenubarState *state = (MenubarState *) app->states[STATE_MENUBAR];
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

	if(app->ch == 'k') {
		state->button_selected++;
	}

	if(app->ch == 'j') {
		state->button_selected--;
	}

	if(state->button_selected < 0) {
		state->button_selected = 0;
	} 

	if(state->button_selected >= NUM_BUTTONS) {
		state->button_selected = NUM_BUTTONS - 1;
	}

	Menubar_Draw(app);
}
