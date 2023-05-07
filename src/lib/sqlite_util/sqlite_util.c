#include "sqlite_util.h"

int open()
{
	int ret = sqlite3_open(db_filename, &ppdb);
	if(ret != SQLITE_OK)
	{
		perror("Open database Failed!\n");
		exit(ERROR);
	}
	printf("Open Database successfully\n");
	return OK;
}

int exec_sql(
	const char* sql_str,
	int (*callback)(void* data, int argc, char** argv, char** azColName),
	void* data)
{
	zErrMsg = 0;
	int rc = sqlite3_exec(ppdb, sql_str, callback, data, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return ERROR;
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
	}
	return OK;
}
int create_table(const char* sql_str)
{
	return exec_sql(sql_str, NULL, NULL);
}

int add(const char* sql_str)
{
	return exec_sql(sql_str, NULL, NULL);
}

int del(const char* sql_str)
{
	return exec_sql(sql_str, NULL, NULL);
}

int query(const char* sql_str, struct db_row* rows)
{
	row_tick = 0;
	int query_ret = exec_sql(sql_str, select_common_callback, rows);
	return query_ret;
}

int close()
{
	sqlite3_close(ppdb);

	return OK;
}
