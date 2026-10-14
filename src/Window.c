#include "Window.h"

#include <string.h>

void Window_Create(Window *window, int x, int y, int width, int height) {
	if(window->win != NULL) {
		Window_Destroy(window);
	}

	window->width = width;
	window->height = height;

	window->win = newwin(height, width, y, x);
}

void Window_Destroy(Window *window) {
	delwin(window->win);
}
