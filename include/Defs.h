#ifndef DEFS_H
#define DEFS_H

#define FOR_STATE_LIST(DO) \
	DO(STATE_CATALOG, CatalogState, Catalog_Update) \
	DO(STATE_MENUBAR, MenubarState, Menubar_Update) \
	DO(STATE_ADDGAME, AddGameState, AddGame_Update) \
	DO(STATE_LOGO,    int,          Logo_Update)

#endif
