#include "Widgets.h"
#include "App.h"

static int * BarGraph_Convert2ndColumnToInteger(Table *table, int num_rows_graph);
static int BarGraph_GetDataMaxValue(const int *data, int n);

static int * BarGraph_Convert2ndColumnToInteger(Table *table, int num_rows_graph) {
	int *data = malloc(num_rows_graph * sizeof(int));

	for(int i = 0; i < num_rows_graph; i++) {
		data[i] = atoi(table->str[table->ncol * (i + 1) + 1]);
	}

	return data;
}

static int BarGraph_GetDataMaxValue(const int *data, int n) {
	int max_value = data[0];

	for(int i = 1; i < n; i++) {
		if(max_value < data[i]) {
			max_value = data[i];
		}
	}

	return max_value;
}

void Widget_HorizontalBarGraph(WINDOW *win, Table *table, int num_rows_graph, int desc_width, int num_width) {
	if(table->ncol != 2) return;

	if(num_rows_graph >= table->nrow) {
		num_rows_graph = table->nrow - 1;
	}

	static const int graph_spacing = 2;
	static const int graph_padding_left = 10;
	static const int graph_padding_right = 10;

	int *data = BarGraph_Convert2ndColumnToInteger(table, num_rows_graph);
	const int max_data_size = BarGraph_GetDataMaxValue(data, num_rows_graph);
	const int max_graph_width = getmaxx(win) - graph_padding_left - graph_padding_right;

	int start_y = getcury(win);

	int graph_height = graph_spacing * num_rows_graph - 1;

	wattrset(win, A_NORMAL);

	for(int i = 0; i < graph_height; i++) {
		wmove(win, start_y + i, graph_padding_left);
		waddch(win, ACS_VLINE);
	}

	for(int i = 0; i < num_rows_graph; i++) {
		const char *label = table->str[table->ncol * (i + 1)];
		const char *number = table->str[table->ncol * (i + 1) + 1];

		int current_y = start_y + i * graph_spacing;
		int current_width = data[i] * max_graph_width / max_data_size;

		wmove(win, current_y, graph_padding_left - strlen(label));
		wattrset(win, A_NORMAL);
		wprintw(win, "%s", label);

		wmove(win, current_y, graph_padding_left + 1);
		wattrset(win, A_REVERSE);

		Widget_Space(win, current_width);

		wattrset(win, A_NORMAL);
		waddch(win, ' ');
		wprintw(win, "%s", number);
	}


	//for(int i = 0;)
	//


	/*
	int max_graph_width = getmaxx(win) - 2 * padding - desc_width - num_width - 1;
	int max_data_size = BarGraph_GetDataMaxValue(data, num_rows_graph);
	int start_y = getcury(win);

	int graph_height = 2 * num_rows_graph - 1;

	for(int i = 0; i < graph_height; i++) {
		wmove(win, start_y + i, padding + desc_width + 1);
		wattrset(win, A_NORMAL);
		//Widget_Space(win, 1);
		waddch(win, ACS_VLINE);
	}

	for(int i = 0; i < num_rows_graph; i++) {
		wmove(win, start_y + 2 * i, padding);

		//wattrset(win, A_REVERSE | A_DIM);
		wattrset(win, A_NORMAL);
		Widget_Space(win, 1);
		Widget_Label(win, table->str[table->ncol * (i + 1)], desc_width);

		int current_width = data[i] * max_graph_width / max_data_size;

		wmove(win, getcury(win), getcurx(win) + 1);

		//wattrset(win, A_REVERSE | A_DIM);
		wattrset(win, A_REVERSE);

		for(int j = 1; j < current_width; j++) {
			waddch(win, ACS_RTEE);
		}

		//Widget_Space(win, max_graph_width - current_width);

		Widget_Space(win, current_width);

		wattrset(win, A_NORMAL);
		Widget_Space(win, 1);
		Widget_Label(win, table->str[table->ncol * (i + 1) + 1], num_width);
	}
	*/

	free(data);
}
