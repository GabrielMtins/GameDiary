#ifndef WIDGETS_H
#define WIDGETS_H

#include <curses.h>

#define MAX_LINE_SIZE 256
#define FORM_MAX_INPUTS 10

typedef struct {
	char str[MAX_LINE_SIZE];
	int len;

	int offset;
	int cursor_pos;
	int width;

	int bg_attron;
	int cursor_attron;
} LineWidgetData;

typedef struct {
	LineWidgetData inputs[FORM_MAX_INPUTS];
	int input_selected;

	int num_inputs;

	const char *form_title;
	const char **labels;
	int labels_width;

	void (*cancel_button_callback)(void *);
	void (*ok_button_callback)(void *);

	void *callback_args;
} FormWidgetData;

/* Se width = -1, preencher até o final da linha. */
void Widget_Space(WINDOW *win, int width);

/* Se width = -1, o comprimento será o tamanho da string. */
void Widget_Label(WINDOW *win, const char *label, int width);

void Widget_Button(WINDOW *win, const char *label, int width, int selected);

void Widget_InputLine(WINDOW *win, const LineWidgetData *data);

void LineWidgetData_HandleInput(LineWidgetData *data, int ch);

void Widget_Form(WINDOW *win, FormWidgetData *data, int ch);

#endif
