#include "App.h"
#include "ini.h"
#include <sys/stat.h>

typedef struct Config {
	App *app;
	char *backup_filename;
	char *db_filename;
} Config;

static int Config_Handler(void *user, const char *section, const char *name, const char *value);
static char * Config_GetConfigFilename(void);
static void Config_GetDefaultDbPath(Config *config);

#define match_str(a, b) (strcmp(a, b) == 0)

static int Config_Handler(void *user, const char *section, const char *name, const char *value) {
	Config *cfg = (Config *) user;

	if(!match_str(section, "application")) {
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
	else if(match_str(name, "database-filename")) {
		cfg->db_filename = sqlite3_mprintf("%s", value);
	}
	else if(match_str(name, "backup-filename")) {
		cfg->backup_filename = sqlite3_mprintf("%s", value);
	}

	return 1;
}

static char * Config_GetConfigDirectory(void) {
	char *home = getenv("HOME");

	return sqlite3_mprintf(
			"%s/" APP_CFG_DIR,
			home
			);
}

static char * Config_GetConfigFilename(void) {
	char *home = getenv("HOME");

	return sqlite3_mprintf(
			"%s/" APP_CFG_DIR "/config.ini",
			home
			);
}

static void Config_GetDefaultDbPath(Config *config) {
	char *home = getenv("HOME");

	if(config->db_filename == NULL) {
		config->db_filename = sqlite3_mprintf("%s/" APP_CFG_DIR "/data.db", home);
	}

	if(config->backup_filename == NULL) {
		config->backup_filename = sqlite3_mprintf("%s/" APP_CFG_DIR "/backup.db", home);
	}
}

void App_Config(App *app) {
	Config config;

	config.app = app;
	config.db_filename = NULL;
	config.backup_filename = NULL;

	char *cfg_dir = Config_GetConfigDirectory();
	char *cfg_filename = Config_GetConfigFilename();

	mkdir(cfg_dir, 0755);

	ini_parse(cfg_filename, Config_Handler, &config);
	Config_GetDefaultDbPath(&config);

	app->db = DB_Connect(config.db_filename, config.backup_filename);

	sqlite3_free(config.db_filename);
	sqlite3_free(config.backup_filename);
	sqlite3_free(cfg_filename);
}
