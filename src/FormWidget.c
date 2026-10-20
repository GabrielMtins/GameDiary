#include "Widgets.h"

#include "App.h"

#define FORM_WIDGET_NUM_BUTTONS 2
#define FORM_END_LABEL_X (2 + data->labels_width)

#define FORM_BUTTON_CANCEL (data->num_inputs + 1)
#define FORM_BUTTON_OK (data->num_inputs + 0)

#define form_label_start(i) 2 + i * 2, 2
#define form_label_end(i, labels_width) 2 + i * 2, 2 + labels_width

static void FormWidget_DrawClear(WINDOW *win, const FormWidgetData *data);
static void FormWidget_DrawInput(WINDOW *win, FormWidgetData *data);
static void FormWidget_DrawButton(WINDOW *win, const FormWidgetData *data);
static void FormWidget_SetUpInputWidget(WINDOW *win, FormWidgetData *data);
static void FormWidget_HandleInput(FormWidgetData *data, int ch);

static void FormWidget_DrawClear(WINDOW *win, const FormWidgetData *data) {
	Style_SetAttr(win, STYLE_FORM_HEADER);

	wmove(win, 0, 0);
	Widget_Label(win, data->form_title, -1);
	Widget_Space(win, -1);

	Style_SetAttr(win, STYLE_FORM_LABEL);

	for(int i = 0; i < data->num_inputs; i++) {
		wmove(win, form_label_start(i));

		Widget_Label(win, data->labels[i], data->labels_width);
	}
}

static void FormWidget_DrawInput(WINDOW *win, FormWidgetData *data) {
	for(int i = 0; i < data->num_inputs; i++) {
		wmove(win, form_label_end(i, data->labels_width));

		data->inputs[i].bg_color = STYLE_FORM_INPUT_BG;
		data->inputs[i].cursor_color = STYLE_FORM_INPUT_BG;

		if(i == data->input_selected) {
			data->inputs[i].cursor_color = STYLE_FORM_INPUT_CURSOR;
		}

		Widget_InputLine(win, &data->inputs[i]);
	}
}

static void FormWidget_DrawButton(WINDOW *win, const FormWidgetData *data) {
	const int line = 2 + data->num_inputs * 2;

	static const int button_width = 10;
	const int middle_screen = getmaxx(win) / 2;

	wmove(win, line + 2, middle_screen - button_width / 2);
	Widget_Button(
			win,
			"  Cancel  ",
			-1,
			data->input_selected == (FORM_BUTTON_CANCEL)
			);

	wmove(win, line, middle_screen - button_width / 2);
	Widget_Button(
			win,
			"    OK    ",
			-1,
			data->input_selected == (FORM_BUTTON_OK)
			);
}

static void FormWidget_SetUpInputWidget(WINDOW *win, FormWidgetData *data) {
	for(int i = 0; i < data->num_inputs; i++) {
		data->inputs[i].width = getmaxx(win) - FORM_END_LABEL_X - 2;
	}
}

static void FormWidget_HandleInput(FormWidgetData *data, int ch) {
	switch(ch) {
		case KEY_ESCAPE:
			if(data->cancel_callback == NULL) break;

			data->cancel_callback(data->callback_args);
			break;

		case '\n':
			if(data->input_selected == FORM_BUTTON_CANCEL) {
				if(data->cancel_callback == NULL) break;

				data->cancel_callback(data->callback_args);
			} else if(data->input_selected == FORM_BUTTON_OK) {
				if(data->ok_callback == NULL) break;

				data->ok_callback(data->callback_args);
			} else {
				data->input_selected++;
			}
			break;

		case KEY_DOWN:
		case '\t':
			data->input_selected++;
			break;

		case KEY_UP:
		case KEY_BTAB:
			data->input_selected--;
			break;
	}

	LIMIT_VAR(data->input_selected, 0, data->num_inputs + FORM_WIDGET_NUM_BUTTONS);

	for(int i = 0; i < data->num_inputs; i++) {
		LineWidgetData_ApplyLimit(&data->inputs[i]);
	}

	if(data->input_selected < data->num_inputs) {
		LineWidgetData_HandleInput(
				&data->inputs[data->input_selected],
				ch
				);
	}
}

void Widget_Form(WINDOW *win, FormWidgetData *data, int ch) {
	if(ch == KEY_RESIZE || data->should_clear) {
		FormWidget_SetUpInputWidget(win, data);
		FormWidget_DrawClear(win, data);
		
		data->should_clear = false;
	}

	FormWidget_HandleInput(data, ch);

	FormWidget_DrawInput(win, data);
	FormWidget_DrawButton(win, data);
}

void FormWidgetData_Init(FormWidgetData *data) {
	memset(data, 0, sizeof(FormWidgetData));

	for(int i = 0; i < data->num_inputs; i++) {
		LineWidgetData_Init(&data->inputs[i]);
	}
}

void FormWidgetData_ResetInput(FormWidgetData *data) {
	for(int i = 0; i < data->num_inputs; i++) {
		data->inputs[i].len = 0;
	}

	data->input_selected = 0;
}
