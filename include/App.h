#ifndef APP_H
#define APP_H

#include "Window.h"
#include "Table.h"
#include "Widgets.h"
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>

#define KEY_ESCAPE 27

#define FOR_STATE_LIST(DO) \
	DO(STATE_CATALOG, CatalogState, Catalog_Update) \
	DO(STATE_LOGVIEW, LogViewState, LogView_Update) \
	DO(STATE_MENUBAR, MenubarState, Menubar_Update) \
	DO(STATE_ADDGAME, AddGameState, AddGame_Update) \
	DO(STATE_ADDLOG,  AddLogState,  AddLog_Update ) \
	DO(STATE_LOGO,    int,          Logo_Update)

#define LIMIT_VAR(x, min_x, max_x) (x = (x < (min_x)) ? (min_x) : ((x >= (max_x)) ? (max_x - 1) : x))

typedef struct CatalogState {
	int input_state;

	TableWidgetData table_data;
} CatalogState;

typedef struct {
	int input_state;

	TableWidgetData table_data;
} LogViewState;

typedef struct MenubarState {
	int button_selected;
} MenubarState;

typedef struct AddGameState {
	FormWidgetData form_data;
	const char *error_msg;
} AddGameState;

#define LOG_STR_SIZE 256
typedef struct AddLogState {
	FormWidgetData form_data;

	char form_title_data[LOG_STR_SIZE];
	char chosen_game[LOG_STR_SIZE];

	const char *error_msg;
} AddLogState;

typedef struct App App;

void App_Init(App *app);

void App_Run(App *app);

void App_End(App *app);

void App_ClearBottomWindow(App *app);

bool App_IsNewState(App *app);

void AddLog_SetGame(App *app, const char *str);

/* ============= MACROS EXPANDING ============= */

#define EXPAND_AS_FUNCTION(type, state, update) void update(App *app);
FOR_STATE_LIST(EXPAND_AS_FUNCTION)
#undef EXPAND_AS_FUNCTION

#define EXPAND_AS_ENUM(type, ...) type,
typedef enum {
	FOR_STATE_LIST(EXPAND_AS_ENUM)
	NUM_STATES,
	STATE_NONE
} StateType;
#undef EXPAND_AS_ENUM

struct App {
	Window content_window;
	Window top_window;
	Window bottom_window;
	Window logo_window;

	void *states[NUM_STATES];

	sqlite3 *db;

	int prev_state;
	int current_state;
	int next_state;

	int quit;
	int prev_ch;
	int ch;
};

#endif
