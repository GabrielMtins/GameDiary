#include "Widgets.h"

#include "App.h"

#define FORM_WIDGET_NUM_BUTTONS 2
#define FORM_END_LABEL_X(labels_width) (2 + labels_width)

#define FORM_BUTTON_CANCEL (data->num_inputs + 0)
#define FORM_BUTTON_OK (data->num_inputs + 1)

#define form_label_start(i) 2 + i * 2, 2
#define form_label_end(i, labels_width) 2 + i * 2, 2 + labels_width

static void FormWidget_DrawClear(WINDOW *win, const FormWidgetData *data);
static void FormWidget_DrawInput(WINDOW *win, FormWidgetData *data);
static void FormWidget_DrawButton(WINDOW *win, const FormWidgetData *data);
static void FormWidget_HandleInput(FormWidgetData *data, int ch);

static void FormWidget_DrawClear(WINDOW *win, const FormWidgetData *data) {
	wclear(win);
	wattrset(win, A_REVERSE);

	{
		wmove(win, 0, 0);
		Widget_Label(win, data->form_title, -1);
		Widget_Space(win, -1);
	}

	for(int i = 0; i < data->num_inputs; i++) {
		wmove(win, form_label_start(i));

		Widget_Label(win, data->labels[i], data->labels_width);
	}
}

static void FormWidget_DrawInput(WINDOW *win, FormWidgetData *data) {
	wattrset(win, A_REVERSE | A_DIM);

	for(int i = 0; i < data->num_inputs; i++) {
		wmove(win, form_label_end(i, data->labels_width));

		data->inputs[i].width = getmaxy(win) - FORM_END_LABEL_X(data->labels_width) - 2;

		if(i == data->input_selected) {
			data->inputs[i].cursor_attron = A_BOLD | A_REVERSE;
		} else {
			data->inputs[i].cursor_attron = data->inputs[i].bg_attron;
		}

		Widget_InputLine(win, &data->inputs[i]);
	}
}

static void FormWidget_DrawButton(WINDOW *win, const FormWidgetData *data) {
	const int line = 2 + data->num_inputs * 2;

	static const int button_padding = 5;
	static const int button_width = 10;
	const int middle_screen = getmaxy(win) / 2;

	wmove(win, line, middle_screen - button_padding - button_width);
	Widget_Button(
			win,
			"  Cancel  ",
			-1,
			data->input_selected == (FORM_BUTTON_CANCEL)
			);

	wmove(win, line, middle_screen + button_padding);
	Widget_Button(
			win,
			"    OK    ",
			-1,
			data->input_selected == (FORM_BUTTON_OK)
			);
}

static void FormWidget_HandleInput(FormWidgetData *data, int ch) {
	if(ch == '\n' && data->input_selected == FORM_BUTTON_CANCEL) {
		if(data->cancel_button_callback != NULL) {
			data->cancel_button_callback(data->callback_args);
		}
	}

	if(ch == '\n' && data->input_selected == FORM_BUTTON_OK) {
		if(data->ok_button_callback != NULL) {
			data->ok_button_callback(data->callback_args);
		}
	}

	if(ch == KEY_DOWN || ch == '\n' || ch == '\t') {
		data->input_selected++;
	}

	if(ch == KEY_UP || ch == KEY_BTAB) {
		data->input_selected--;
	}

	LIMIT_VAR(data->input_selected, 0, data->num_inputs + FORM_WIDGET_NUM_BUTTONS);

	if(data->input_selected < data->num_inputs) {
		LineWidgetData_HandleInput(
				&data->inputs[data->input_selected],
				ch
				);
	}
}

void Widget_Form(WINDOW *win, FormWidgetData *data, int ch) {
	if(ch == KEY_RESIZE) {
		FormWidget_DrawClear(win, data);
	}

	FormWidget_HandleInput(data, ch);

	FormWidget_DrawInput(win, data);
	FormWidget_DrawButton(win, data);
}
