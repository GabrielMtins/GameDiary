#include "Widgets.h"
#include "App.h"

static int * BarGraph_Convert2ndColumnToInteger(Table *table, int num_rows_graph);
static int BarGraph_GetDataMaxValue(const int *data, int n);
static int BarGraph_GetMaxNameLen(Table *table, int num_rows_graphs);

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

static int BarGraph_GetMaxNameLen(Table *table, int num_rows_graphs) {
	int max_len = 0;
	int current;

	for(int i = 0; i < num_rows_graphs; i++) {
		current = strlen(table->str[table->ncol * (i + 1)]);

		if(current > max_len) max_len = current;
	}

	return max_len;
}

void Widget_HorizontalBarGraph(WINDOW *win, Table *table, int num_rows_graph) {
	if(table->ncol != 2) return;

	if(num_rows_graph >= table->nrow) {
		num_rows_graph = table->nrow - 1;
	}

	int *data = BarGraph_Convert2ndColumnToInteger(table, num_rows_graph);
	int max_data_size = BarGraph_GetDataMaxValue(data, num_rows_graph);
	int max_name_len = BarGraph_GetMaxNameLen(table, num_rows_graph);

	static const int graph_spacing = 2;
	static const int graph_padding_right = 8;

	int max_possible_padding = getmaxx(win) / 5;
	int graph_padding_left = max_name_len + 2;

	if(graph_padding_left > max_possible_padding) graph_padding_left = max_possible_padding;

	int max_graph_width = getmaxx(win) - graph_padding_left - graph_padding_right;

	int start_y = getcury(win);

	int graph_height = graph_spacing * num_rows_graph - 1;

	Style_SetAttr(win, STYLE_HGRAPH_VLINE);

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
		Style_SetAttr(win, STYLE_HGRAPH_LABEL);
		wprintw(win, "%s", label);

		wmove(win, current_y, graph_padding_left + 1);
		Style_SetAttr(win, STYLE_HGRAPH_BAR);
		Widget_Space(win, current_width);

		wmove(win, current_y, getcurx(win) + 1);
		Style_SetAttr(win, STYLE_HGRAPH_NUMBER);
		wprintw(win, "%s", number);
	}

	free(data);
}
