#include "DbCmd.h"

#include <stddef.h>
#include <string.h>

static const char *sql_cmd_create_tables = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"id INTEGER PRIMARY KEY,"
"Name VARCHAR(120) UNIQUE NOT NULL COLLATE NOCASE,"
"Genre VARCHAR(32) COLLATE NOCASE,"
"Platform VARCHAR(32) COLLATE NOCASE,"
"Year INTEGER"
");"

"CREATE TABLE IF NOT EXISTS LogTable ("
"id INTEGER PRIMARY KEY,"
"GameId INTEGER NOT NULL,"
"StartPlayDate CHAR(10),"
"EndPlayDate CHAR(10),"
"Score INTEGER,"
"FOREIGN KEY (GameId) REFERENCES GameTable(id) "
"ON DELETE CASCADE "
"ON UPDATE CASCADE"
");"
;

static const char *sql_cmd_insert_game = 
"INSERT INTO GameTable (Name, Genre, Platform, Year) VALUES (%Q, %Q, %Q, %d);"
;

static const char *sql_cmd_delete_game =
"DELETE FROM GameTable WHERE Name=%Q;"
;

static const char *sql_cmd_update_game =
"UPDATE GameTable "
"SET Name=%Q, Genre=%Q, Platform=%Q, Year=%d "
"WHERE Name=%Q;"
;

static const char *sql_cmd_get_game_table = 
"SELECT Name, Genre, Platform, Year FROM GameTable ORDER BY Name ASC;"
;

static const char *sql_cmd_search_game = 
"SELECT Name, Genre, Platform, Year FROM GameTable WHERE Name LIKE %Q ORDER BY Name ASC;"
;

static const char *sql_cmd_insert_log =
"INSERT INTO LogTable (GameId, StartPlayDate, EndPlayDate, Score) VALUES "
"((SELECT id FROM GameTable Where Name=%Q),"
"%Q, %Q, %d);"
;

static const char *sql_cmd_delete_log =
"DELETE FROM LogTable WHERE id=%s;"
;

static const char *sql_cmd_update_log =
"UPDATE LogTable "
"SET StartPlayDate=%Q, EndPlayDate=%Q, Score=%d "
"WHERE id=%s;"
;

static const char *sql_cmd_get_log_table =
"SELECT "
"l.id as id, "
"g.Name AS Name," 
"l.StartPlayDate AS [Start Date],"
"l.EndPlayDate AS [End Date],"
"l.Score "
"FROM GameTable g "
"INNER JOIN LogTable l on g.id = l.GameId "
"ORDER BY l.StartPlayDate DESC;"
;

static const char *sql_cmd_get_platform_count =
"SELECT g.Platform as Platform, COUNT(*) as qt "
"FROM (SELECT DISTINCT GameId FROM LogTable) l "
"INNER JOIN GameTable g on g.id = l.GameId "
"GROUP BY g.Platform "
"ORDER BY qt DESC;"
;

static const char *sql_cmd_get_genre_count =
"SELECT g.Genre, COUNT(*) as qt "
"FROM (SELECT DISTINCT GameId FROM LogTable) l "
"INNER JOIN GameTable g on g.id = l.GameId "
"GROUP BY g.Genre "
"ORDER BY qt DESC;"
;

static void DB_Exec(sqlite3 *db, const char *fmt, ...);
static void DB_GetTable(sqlite3 *db, Table *table, const char *fmt, ...);

static void DB_Exec(sqlite3 *db, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char *cmd = sqlite3_vmprintf(fmt, args);

	sqlite3_exec(db, cmd, NULL, NULL, NULL);

	sqlite3_free(cmd);

	va_end(args);
}

static void DB_GetTable(sqlite3 *db, Table *table, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char *cmd = sqlite3_vmprintf(fmt, args);

	if(table->str != NULL) DB_DestroyTable(table);

	sqlite3_get_table(
			db,
			cmd,
			&table->str,
			&table->nrow,
			&table->ncol,
			NULL
			);

	table->nrow++;
}

void DB_Connect(sqlite3 **db) {
	sqlite3_open("data.db", db);

	DB_Exec(*db, "%s", "PRAGMA foreign_keys = ON;");
	DB_Exec(*db, "%s", sql_cmd_create_tables);
}

void DB_Quit(sqlite3 *db) {
	sqlite3_close(db);
}

void DB_DestroyTable(Table *table) {
	if(table->str != NULL) {
		sqlite3_free_table(table->str);
	}
}

void DB_InsertGame(sqlite3 *db, const char *name, const char *genre, const char *platform, int year) {
	DB_Exec(
			db,
			sql_cmd_insert_game,
			name,
			genre,
			platform,
			year
		   );
}

void DB_DeleteGame(sqlite3 *db, const char *name) {
	DB_Exec(
			db,
			sql_cmd_delete_game,
			name
		   );
}

void DB_UpdateGame(sqlite3 *db, const char *old_name, const char *name, const char *genre, const char *platform, int year) {
	DB_Exec(
			db,
			sql_cmd_update_game,
			name,
			genre,
			platform,
			year,
			old_name
		   );
}

void DB_GetGameTable(sqlite3 *db, Table *table) {
	DB_GetTable(
			db,
			table,
			sql_cmd_get_game_table
			);
}

void DB_SearchGame(sqlite3 *db, Table *table, const char *name) {
	if(name[0] == '\0') {
		DB_GetGameTable(db, table);
		return;
	}

	char *tmp = sqlite3_mprintf("%%%s%%", name);

	DB_GetTable(
			db,
			table,
			sql_cmd_search_game,
			tmp
			);

	sqlite3_free(tmp);
}

void DB_InsertLog(sqlite3 *db, const char *name, const char *start_date, const char *end_date, int score) {
	DB_Exec(
			db,
			sql_cmd_insert_log,
			name,
			start_date,
			end_date,
			score
		   );
}

void DB_DeleteLog(sqlite3 *db, const char *id) {
	DB_Exec(
			db,
			sql_cmd_delete_log,
			id
		   );
}

void DB_UpdateLog(sqlite3 *db, const char *start_date, const char *end_date, int score, const char *id) {
	DB_Exec(
			db,
			sql_cmd_update_log,
			start_date,
			end_date,
			score,
			id
		   );
}

void DB_GetLogTable(sqlite3 *db, Table *table) {
	DB_GetTable(
			db,
			table,
			sql_cmd_get_log_table
			);
}

void DB_GetStatsPlatform(sqlite3 *db, Table *table) {
	DB_GetTable(
			db,
			table,
			sql_cmd_get_platform_count
			);
}
