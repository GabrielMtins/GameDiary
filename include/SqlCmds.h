#ifndef CMDS_H
#define CMDS_H

static const char *sql_cmd_create_tables = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"Name VARCHAR(120) PRIMARY KEY,"
"Genre VARCHAR(120),"
"Platform VARCHAR(64),"
"Year YEAR"
");"

"CREATE TABLE IF NOT EXISTS LogTable ("
"GameName INT NOT NULL REFERENCES GameTable(Name),"
"StartPlayDate CHAR(10),"
"EndPlayDate CHAR(10),"
"Stars INT"
");"
;

static const char *sql_cmd_insert_game = 
"INSERT INTO GameTable (Name, Genre, Platform, Year) VALUES (\'%s\', \'%s\', \'%s\', \'%s\');"
;

static const char *sql_cmd_delete_game =
"DELETE FROM GameTable WHERE Name=\'%s\';"
;

static const char *sql_cmd_get_game_table = 
"SELECT * FROM GameTable ORDER BY Name ASC;"
;

static const char *sql_cmd_get_log_table =
"SELECT * FROM LogTable ORDER BY StartPlayDate ASC;"
;

#endif
