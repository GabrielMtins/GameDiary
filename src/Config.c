#include "App.h"
#include "ini.h"
#include <sys/stat.h>

static int Config_Handler(void *user, const char *section, const char *name, const char *value);
static char * Config_GetConfigFilename(void);

#define match_str(a, b) (strcmp(a, b) == 0)

static int Config_Handler(void *user, const char *section, const char *name, const char *value) {
	(void) user;

	if(!match_str(section, "SaveState")) {
		return 0;
	}

	if(match_str(name, "colorscheme")) {
		if(match_str(value, "catppuccin")) {
			Style_SetCatppuccinTheme();
		} else if(match_str(value, "default")) {
			Style_SetDefaultTheme();
		} else if(match_str(value, "basic-color")) {
			Style_SetBasicColorTheme();
		}
	}

	return 1;
}

static char * Config_GetConfigDirectory(void) {
	const char *home = getenv("HOME");

	if(home == NULL) home = ".";

	return sqlite3_mprintf(
			APP_CFG_DIR,
			home
			);
}

static char * Config_GetConfigFilename(void) {
	const char *home = getenv("HOME");

	if(home == NULL) home = ".";

	return sqlite3_mprintf(
			APP_CFG_FILENAME,
			home
			);
}

void App_Config(App *app) {
	char *cfg_dir = Config_GetConfigDirectory();
	char *cfg_filename = Config_GetConfigFilename();

	mkdir(cfg_dir, 0755);

	ini_parse(cfg_filename, Config_Handler, app);

	sqlite3_free(cfg_dir);
	sqlite3_free(cfg_filename);
}
