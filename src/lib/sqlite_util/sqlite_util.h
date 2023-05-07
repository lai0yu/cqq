#ifndef __SQLITE_UTIL_H__
#define __SQLITE_UTIL_H__

#include "../linux_list/linux_list.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1

static int row_tick = 0;
static sqlite3* ppdb;
static char* zErrMsg = NULL;

struct dbrow
{
	int argc;
	char** argv;
	char** azColName;
	struct list_head list;
};

static int select_common_callback(void* data, int argc, char** argv, char** azColName)
{
	struct dbrow* rows = (struct dbrow*)data;

	struct dbrow* row = (struct dbrow*)malloc(sizeof(struct dbrow));
	row->argc = argc;
	row->argv = argv;
	row->azColName = azColName;

	list_add(&row->list, &rows->list);

	row_tick++;

	return 0;
}

extern int open_db(const char* db_filename);
extern int exec_sql(
	const char* sql_str,
	int (*callback)(void* data, int argc, char** argv, char** azColName),
	void* data);
extern int create_table(const char* sql_str);
extern int add(const char* sql_str);
extern int del(const char* sql_str);
extern int query(const char* sql_str, struct dbrow* rows);
extern int update(const char* sql_str);
extern int close_db();
#endif
