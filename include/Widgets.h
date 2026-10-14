#ifndef WIDGETS_H
#define WIDGETS_H

#include <curses.h>

#define MAX_LINE_SIZE 256

typedef struct {
	char str[MAX_LINE_SIZE];
	int len;

	int offset;
	int cursor_pos;
	int width;

	int bg_attron;
	int cursor_attron;
} LineWidgetData;

/* Se width = -1, preencher até o final da linha. */
void Widget_Space(WINDOW *win, int width);

/* Se width = -1, o comprimento será o tamanho da string. */
void Widget_Label(WINDOW *win, const char *label, int width);

void Widget_Button(WINDOW *win, const char *label, int width, int selected);

void Widget_InputLine(WINDOW *win, const LineWidgetData *data);

void LineWidgetData_HandleInput(LineWidgetData *data, int ch);

#endif
