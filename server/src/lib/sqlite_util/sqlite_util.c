#include "sqlite_util.h"

int open_db()
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

int close_db()
{
	sqlite3_close(ppdb);

	return OK;
}
