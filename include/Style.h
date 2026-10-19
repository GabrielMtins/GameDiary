#ifndef STYLE_H
#define STYLE_H

#include <curses.h>

#define FOR_STYLE_LIST(DO) \
	DO(STYLE_TABLE_HEADER,      A_REVERSE) \
	DO(STYLE_TABLE_ROW,         A_NORMAL) \
	DO(STYLE_TABLE_SELECTED,    A_REVERSE | A_BOLD) \
	DO(STYLE_FORM_HEADER,       A_REVERSE) \
	DO(STYLE_FORM_LABEL,        A_REVERSE) \
	DO(STYLE_FORM_INPUT_BG,     A_REVERSE | A_DIM) \
	DO(STYLE_FORM_INPUT_CURSOR, A_REVERSE | A_BOLD) \
	DO(STYLE_END, 0)

#define EXPAND_AS_ENUM(id, attr1) id,
typedef enum {
	FOR_STYLE_LIST(EXPAND_AS_ENUM)
} StyleColor;
#undef EXPAND_AS_ENUM

int Style_GetAttr(StyleColor color);


void Style_SetAttr(WINDOW *win, StyleColor color);


#endif
