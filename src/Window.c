#include "Window.h"

#include <string.h>

void Window_Create(Window *window, int x, int y, int width, int height) {
	window->width = width;
	window->height = height;

	window->win = newwin(height, width, y, x);
}

void Window_Destroy(Window *window) {
	delwin(window->win);
}
