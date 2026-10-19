#include "Style.h"

#define EXPAND_AS_ARRAY(id, attr1) (attr1),
static const int style_bw[] = {
	FOR_STYLE_LIST(EXPAND_AS_ARRAY)
};
#undef EXPAND_AS_ARRAY

int Style_GetAttr(StyleColor color) {
	return style_bw[color];
}

void Style_SetAttr(WINDOW *win, StyleColor color) {
	wattrset(win, style_bw[color]);
}
