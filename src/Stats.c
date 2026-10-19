#include "App.h"

void Stats_Update(App *app) {
	if(!(App_IsNewState(app) || app->ch == KEY_RESIZE)) {
		return;
	}

	WINDOW *win = app->content_window;
	wclear(win);
	wmove(win, 0, 0);

	Style_SetAttr(win, STYLE_STATS_HEADER);
	Widget_Space(win, 2);
	Widget_Label(win, "Most played platforms", -1);
	Widget_Space(win, -1);

	wmove(win, 2, 0);

	Table table;

	memset(&table, 0, sizeof(Table));

	DB_GetStatsPlatform(app->db, &table);

	Widget_HorizontalBarGraph(
			win,
			&table,
			10
			);

	DB_DestroyTable(&table);
}
