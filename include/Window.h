#ifndef WINDOW_H
#define WINDOW_H

#include <curses.h>

typedef struct Window {
	WINDOW *win;
	int width, height;
	int owner_state;
} Window;

void Window_Create(Window *window, int x, int y, int width, int height);

void Window_Destroy(Window *window);

#endif
