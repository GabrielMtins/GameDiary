#ifndef APP_H
#define APP_H

#include "Window.h"
#include "Table.h"
#include <sqlite3.h>

#define FOR_STATE_LIST_DO(DO) \
	DO(STATE_START,   int,         StartState_Update) \
	DO(STATE_CATALOG, CatalogState, Catalog_Update) \
	DO(STATE_MENUBAR, MenubarState, Menubar_Update)

typedef struct CatalogState {
	Table working_table;

	int start_row;
	int row_selected;
} CatalogState;

typedef struct MenubarState {
	int button_selected;
} MenubarState;

typedef struct App App;

void App_Init(App *app);

void App_BuildWindow(App *app);

void App_Run(App *app);

void App_End(App *app);

/* ============= MACROS EXPANDING ============= */

#define EXPAND_AS_FUNCTION(type, state, update) void update(App *app);
FOR_STATE_LIST_DO(EXPAND_AS_FUNCTION)
#undef EXPAND_AS_FUNCTION

#define EXPAND_AS_ENUM(type, ...) type,
typedef enum {
	FOR_STATE_LIST_DO(EXPAND_AS_ENUM)
	NUM_STATES,
	STATE_NONE
} StateType;
#undef EXPAND_AS_ENUM

struct App {
	Window content_window;
	Window top_window;
	Window bottom_window;

	void *states[NUM_STATES];

	sqlite3 *db;

	StateType current_state;

	int quit;
	int ch;
};

#endif
