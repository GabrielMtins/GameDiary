#ifndef CMDS_H
#define CMDS_H

const char *create_tables_cmd = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"Name VARCHAR(120) NOT NULL UNIQUE,"
"Genres VARCHAR(120),"
"Platforms VARCHAR(64),"
"Year YEAR"
");"

"CREATE TABLE IF NOT EXISTS ReviewTable ("
"GameName INT NOT NULL REFERENCES GameTable(Name),"
"StartPlayDate DATE,"
"EndPlayDate DATE,"
"Stars INT"
");"
;

const char *insert_game_cmd = 
"INSERT INTO GameTable (Name, Genres, Platform, Year) VALUES (\'%s\', \'%s\', \'%s\', \'%d\');";

const char *get_game_table_cmd = 
"SELECT * FROM GameTable;"
;

#endif
