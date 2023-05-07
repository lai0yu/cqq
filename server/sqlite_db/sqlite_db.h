#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#define OK 0
#define ERROR -1

static int row_tick = 0;

struct select_row
{
	int argc;
	char** argv;
	char** azColName;
};

static int select_common_callback(void* data, int argc, char** argv, char** azColName)
{
	// struct select_row *rows = (struct select_row *)data;
	// struct select_row row;
	// row.argc = argc;
	// row.argv = argv;
	// row.azColName = azColName;
	// row_tick++;
	return 0;
}

const char* db_filename = "cqq.db";
sqlite3* ppdb;

char* zErrMsg = NULL;

extern int open_db();
extern int exec_sql(
	const char* sql_str,
	int (*callback)(void* data, int argc, char** argv, char** azColName),
	void* data);
extern int create_table(const char* sql_str);
extern int add(const char* sql_str);
extern int del(const char* sql_str);
extern int query(const char* sql_str, struct select_row** rows);
extern int update(const char* sql_str);

extern int close_db();
#endif
