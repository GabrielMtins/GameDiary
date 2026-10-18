#ifndef WIDGETS_H
#define WIDGETS_H

#include <curses.h>

#include "DbCmd.h"

#define MAX_LINE_SIZE 255
#define FORM_MAX_INPUTS 10

typedef struct {
	char str[MAX_LINE_SIZE + 1];
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

	void (*cancel_callback)(void *);
	void (*ok_callback)(void *);

	void *callback_args;

	bool should_clear;
} FormWidgetData;

typedef struct {
	Table working_table;

	int start_row;
	int selected_row;

	int height;

	const char *msg_no_entries;

	const int *col_width_values;
} TableWidgetData;

/* Se width = -1, preencher até o final da linha. */
void Widget_Space(WINDOW *win, int width);

/* Se width = -1, o comprimento será o tamanho da string. */
void Widget_Label(WINDOW *win, const char *label, int width);

void Widget_Button(WINDOW *win, const char *label, int width, int selected);

void Widget_InputLine(WINDOW *win, const LineWidgetData *data);

void LineWidgetData_Init(LineWidgetData *data);

/* DIRTY */
void LineWidgetData_HandleInput(LineWidgetData *data, int ch);

void Widget_Form(WINDOW *win, FormWidgetData *data, int ch);

void FormWidgetData_Init(FormWidgetData *data);

void FormWidgetData_ResetInput(FormWidgetData *data);

void Widget_Table(WINDOW *win, const TableWidgetData *data);

void TableWidgetData_Init(TableWidgetData *data);

void TableWidgetData_HandleInput(TableWidgetData *data, int ch, int prev_ch);

char ** TableWidgetData_GetSelectedRow(const TableWidgetData *data);

void Widget_HorizontalBarGraph(WINDOW *win, Table *table, int num_rows_graph, int desc_width, int num_width);

#endif
