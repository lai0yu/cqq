#include "account_action.h"
#include "../../sqlite_db/sqlite_db.h"

int init_service()
{
	create_tables();
}

static common_c() {}

int create_tables()
{
	return exec_sql(sql_create_account_table, NULL, NULL);
}

int sign_in(const char* username, const char* password)
{
	char full_sql[128];
}

int sign_up(const char* username, const char* password)
{
	char full_sql[128];
	sprintf(full_sql, sql_insert_account, username, password);
	return exec_sql(sql_insert_account, full_sql, NULL);
}