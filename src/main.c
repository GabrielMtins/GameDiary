#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>
#include <curses.h>

#include "App.h"

/* TODO
 * - estatisticas
 * - help/about
 */

/*
 * As funcoes que possuem uma tag DIRTY no seu comentario é porque mexem
 * com memória e necessitam de verificação de dados. 
 */

int main(void) {
	App *app;

	app = (App *) malloc(sizeof(App));

	App_Init(app);
	App_Run(app);
	App_End(app);

	return 0;
}
