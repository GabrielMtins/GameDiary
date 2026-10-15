#ifndef CMDS_H
#define CMDS_H

static const char *sql_cmd_create_tables = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"id INTEGER PRIMARY KEY,"
"Name VARCHAR(120) UNIQUE NOT NULL COLLATE NOCASE,"
"Genre VARCHAR(120) COLLATE NOCASE,"
"Platform VARCHAR(64) COLLATE NOCASE,"
"Year YEAR"
");"

"CREATE TABLE IF NOT EXISTS LogTable ("
"GameId INTEGER NOT NULL,"
"StartPlayDate CHAR(10),"
"EndPlayDate CHAR(10),"
"Score INT,"
"FOREIGN KEY (GameId) REFERENCES GameTable(id) "
"ON DELETE CASCADE "
"ON UPDATE CASCADE"
");"
;

static const char *sql_cmd_insert_game = 
"INSERT INTO GameTable (Name, Genre, Platform, Year) VALUES (\'%s\', \'%s\', \'%s\', \'%s\');"
;

static const char *sql_cmd_delete_game =
"DELETE FROM GameTable WHERE Name=\'%s\';"
;

static const char *sql_cmd_get_game_table = 
"SELECT Name, Genre, Platform, Year FROM GameTable ORDER BY Name ASC;"
;

static const char *sql_cmd_insert_log =
"INSERT INTO LogTable (GameId, StartPlayDate, EndPlayDate, Score) VALUES "
"((SELECT id FROM GameTable Where Name=\'%s\'),"
"\'%s\', \'%s\', \'%s\');"
;

static const char *sql_cmd_get_log_table =
"SELECT "
"g.Name AS Name," 
"l.StartPlayDate AS [Start Date],"
"l.EndPlayDate AS [End Date],"
"l.Score "
"FROM GameTable g "
"INNER JOIN LogTable l on g.id = l.GameId "
"ORDER BY l.StartPlayDate DESC;"
;

#endif
