#include "account.h"

int create_tables()
{
	return exec_sql(sql_create_account_table, NULL, NULL);
}

int init_service()
{
	return create_tables();
}

int sign_in(const char* username, const char* password)
{
	char full_sql[128];
	sprintf(full_sql, sql_select_by_username, username);

	struct db_row head;
	INIT_LIST_HEAD(&head.list);

	int select_ret = query(full_sql, &head);

	int return_ret = -1;
	struct db_row* pos;

	if(select_ret == SQLITE_OK)
	{

		int username_cmp_ret = -1;
		int password_cmp_ret = -1;
		list_for_each_entry(pos, &head.list, list)
		{
			username_cmp_ret = strcmp(username, pos->argv[1]);
			password_cmp_ret = strcmp(password, pos->argv[2]);
			if(username_cmp_ret == 0 && username_cmp_ret == 0)
			{
				//TODO ： Sing in successfully
				return_ret = 0;
				break;
			}
		}
	}

	list_for_each_entry(pos, &head.list, list)
	{
		free(pos);
	}
	list_empty(&head.list);

	return return_ret;
}

int sign_up(const char* username, const char* password)
{
	char full_sql[128];
	sprintf(full_sql, sql_insert_account, username, password);
	return add(full_sql);
}