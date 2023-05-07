#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include "../../linux_list/linux_list.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1

static int row_tick = 0;

struct data_row
{
	int argc;
	char** argv;
	char** azColName;
	struct list_head list_node;
};

int select_common_callback(void* data, int argc, char** argv, char** azColName)
{
	struct list_head* data_list = (struct list_head*)data;

	struct data_row* row = (struct data_row*)malloc(sizeof(struct data_row));
	row->argc = argc;
	row->argv = argv;
	row->azColName = azColName;

	list_add(&row->list_node, data_list);

	row_tick++;

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
extern int query(const char* sql_str, struct data_row* data_list);
extern int update(const char* sql_str);

extern int close_db();
#endif
