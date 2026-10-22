#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>
#include <curses.h>

#include "App.h"

int main(void) {
	App *app;

	app = (App *) malloc(sizeof(App));

	App_Init(app);
	App_Run(app);
	App_End(app);

	return 0;
}
