#ifndef __SQLITE_UTIL_H__
#define __SQLITE_UTIL_H__

#include "../linux_list/linux_list.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 0
#define ERROR -1

static const char* db_filename = "test.db";
static sqlite3* ppdb;
static int row_tick = 0;
static char* zErrMsg = NULL;

struct db_row
{
	struct list_head list;
	int argc;
	char** argv;
	char** azColName;
};

static int select_common_callback(void* data, int argc, char** argv, char** azColName)
{
	struct db_row* rows = (struct db_row*)data;
	struct db_row* row = (struct db_row*)malloc(sizeof(struct db_row));

	row->argc = argc;
	row->argv = (char**)malloc(sizeof(char*) * argc);
	row->azColName = (char**)malloc(sizeof(char*) * argc);
	int i;
	for(i = 0; i < argc; ++i)
	{
		row->argv[i] = (char*)malloc(strlen(argv[i]) + 1);
		row->azColName[i] = (char*)malloc(strlen(azColName[i]) + 1);
		strcpy(row->argv[i], argv[i]);
		strcpy(row->azColName[i], azColName[i]);
		printf("%s:%s\t", azColName[i], argv[i]);
	}
	printf("\n");

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
