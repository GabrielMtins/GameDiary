#ifndef CMDS_H
#define CMDS_H

static const char *sql_cmd_create_tables = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"Name VARCHAR(120) PRIMARY KEY COLLATE NOCASE,"
"Genre VARCHAR(120) COLLATE NOCASE,"
"Platform VARCHAR(64) COLLATE NOCASE,"
"Year YEAR"
");"

"CREATE TABLE IF NOT EXISTS LogTable ("
"GameName VARCHAR(120) NOT NULL,"
"StartPlayDate CHAR(10),"
"EndPlayDate CHAR(10),"
"Score INT,"
"FOREIGN KEY (GameName) REFERENCES GameTable(Name) "
"ON DELETE CASCADE "
"ON UPDATE CASCADE"
");"
;

static const char *sql_cmd_insert_game = 
"INSERT INTO GameTable (Name, Genre, Platform, Year) VALUES (\'%s\', \'%s\', \'%s\', \'%s\');"
;

static const char *sql_cmd_insert_log =
"INSERT INTO LogTable (GameName, StartPlayDate, EndPlayDate, Score) VALUES (\'%s\', \'%s\', \'%s\', \'%s\');"
;

static const char *sql_cmd_delete_game =
"DELETE FROM GameTable WHERE Name=\'%s\';"
;

static const char *sql_cmd_get_game_table = 
"SELECT * FROM GameTable ORDER BY Name ASC;"
;

static const char *sql_cmd_get_log_table =
"SELECT "
"GameName AS Name," 
"StartPlayDate AS [Start Date],"
"EndPlayDate AS [End Date],"
"Score "
"FROM LogTable "
"ORDER BY StartPlayDate DESC;"
;

#endif
