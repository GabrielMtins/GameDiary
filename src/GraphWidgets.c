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

	int *data = BarGraph_Convert2ndColumnToInteger(table, num_rows_graph);
	static int padding = 2;

	int max_graph_width = getmaxx(win) - 2 * padding - desc_width - num_width - 1;
	int max_data_size = BarGraph_GetDataMaxValue(data, num_rows_graph);
	int start_y = getcury(win);

	int graph_height = 2 * num_rows_graph - 1;

	for(int i = 0; i < graph_height; i++) {
		wmove(win, start_y + i, padding + desc_width);
		wattrset(win, A_REVERSE);
		Widget_Space(win, 1);
	}

	for(int i = 0; i < num_rows_graph; i++) {
		wmove(win, start_y + 2 * i, padding);

		wattrset(win, A_NORMAL);
		Widget_Label(win, table->str[table->ncol * (i + 1)], desc_width);

		wattrset(win, A_REVERSE);
		Widget_Space(win, 1);

		int current_width = data[i] * max_graph_width / max_data_size;

		Widget_Space(win, current_width);

		wattrset(win, A_NORMAL);
		Widget_Space(win, 1);
		Widget_Label(win, table->str[table->ncol * (i + 1) + 1], num_width);
	}

	free(data);
}
