#include "App.h"

static const char *about_text[] = {
	"Developed by Gabriel Martins.",
	"GameDiary is a program for logging game data.",
	"This is free software distributed under GNU GPL v3 or later."
};

#define NUM_ABOUT_TEXT ((int)(sizeof(about_text) / sizeof(const char *)))

void About_Update(App *app) {
	if(!(App_IsNewState(app) || app->ch == KEY_RESIZE)) {
		return;
	}

	WINDOW *win = app->content_window;
	int width = getmaxx(win);
	int start_y = (getmaxy(win) - NUM_ABOUT_TEXT) / 2;

	Style_SetAttr(win, STYLE_ABOUT_TEXT);
	for(int i = 0; i < NUM_ABOUT_TEXT; i++) {
		wmove(win, i + start_y, (width - strlen(about_text[i])) / 2);
		Widget_Label(win, about_text[i], -1);
	}
}
