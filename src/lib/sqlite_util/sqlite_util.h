#ifndef __SQLITE_UTIL_H__
#define __SQLITE_UTIL_H__

#include "../linux_list/linux_list.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1

static const char* db_filename = "test.db";
static sqlite3* ppdb;
static int row_tick = 0;
static char* zErrMsg = NULL;

struct db_row
{
	int argc;
	char** argv;
	char** azColName;
	struct list_head list;
};

static int select_common_callback(void* data, int argc, char** argv, char** azColName)
{
	struct db_row* rows = (struct db_row*)data;

	struct db_row* row = (struct db_row*)malloc(sizeof(struct db_row));
	row->argc = argc;
	row->argv = argv;
	row->azColName = azColName;

	list_add(&row->list, &rows->list);

	row_tick++;

	return 0;
}

extern int open_db();
extern int exec_sql(
	const char* sql_str,
	int (*callback)(void* data, int argc, char** argv, char** azColName),
	void* data);
extern int close_db();
#endif
