#ifndef __SQLITE_UTIL_H__
#define __SQLITE_UTIL_H__

#include "../../lib/linux_list/linux_list.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* db_filename = "test.db";
sqlite3* ppdb;

static char fsql[1024];
static const char* select_sql = "select %s from %s where %s";
static const char* insert_sql = "insert    into %s values%s";
static const char* delete_sql = "delete    from %s where %s";
static const char* update_sql = "update %s set  %s where %s";

struct select_row {
	struct list_head list;
	int argc;
	char** argv;
	char** azColName;
	int index;
};

static int db_open() {
	int rc = sqlite3_open("test.db", &ppdb);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(ppdb));
		exit(0);
	} else {
		fprintf(stderr, "Opened database successfully\n");
	}
	return rc;
}

static inline int db_close() { return sqlite3_close(ppdb); }
static int select_callback(void* data, int argc, char** argv, char** azColName) {
	struct select_row* head = (struct select_row*)data;

	struct select_row* new_row = (struct select_row*)malloc(sizeof(struct select_row));
	new_row->argc = argc;
	new_row->argv = (char**)malloc(sizeof(char*) * argc);
	new_row->azColName = (char**)malloc(sizeof(char*) * argc);
	new_row->index = list_entry(head->list.prev, struct select_row,list)->index + 1;

	int i;
	for (i = 0; i < argc; ++i) {
		new_row->argv[i] = (char*)malloc(strlen(argv[i]) + 1);
		new_row->azColName[i] = (char*)malloc(strlen(azColName[i]) + 1);
		strcpy(new_row->argv[i], argv[i]);
		strcpy(new_row->azColName[i], azColName[i]);
		printf("%s:%s\t", azColName[i], argv[i]);
	}
	list_add_tail(&head->list, &new_row->list);
	printf("\n");
	return 0;
}

static int exec_sql(const char* sql_str,
					int (*callback)(void* data, int argc, char** argv, char** azColName),
					void* data) {
	char* zErrMsg;
	printf("%s\n",sql_str);
	int rc = sqlite3_exec(ppdb, sql_str, callback, data, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL ERROR: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Operation done successfully\n");
	}
	return rc;
}

extern int db_insert(const char* table_columns, const char* values);
extern int db_delete(const char* table, const char* where);
extern struct select_row db_select(const char* table, const char* columns, const char* where);
extern int db_update(const char* table, const char* sets, const char* where);
extern void free_select_rows_list(struct select_row* row_head);
#endif
