#include "account.h"

int create_tables()
{
	open_db();
	return exec_sql(sql_create_account_table, NULL, NULL);
}

int init_service()
{
	return create_tables();
}

int sign_in(const char* data, int socket)
{
	struct si_data sid = parse_sign_in_data(data);

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_by_username, sid.username);

	struct db_row head;
	INIT_LIST_HEAD(&head.list);

	int select_ret = exec_sql(fsql, select_common_callback, (void*)&head);

	int return_ret = -1;
	struct db_row* pos;

	if(select_ret == SQLITE_OK)
	{
		int username_cmp_ret = -1;
		int password_cmp_ret = -1;
		list_for_each_entry(pos, &head.list, list)
		{
			username_cmp_ret = strcmp(sid.username, pos->argv[1]);
			password_cmp_ret = strcmp(sid.password, pos->argv[2]);
			if(username_cmp_ret == 0 && username_cmp_ret == 0)
			{
				bzero(fsql, sizeof(fsql));
				sprintf(fsql, sql_update_socket_by_username, socket, sid.username);
				exec_sql(fsql, NULL, NULL);

				struct msg rmsg = {0, "smsg", "登录成功"};
				char msg_buf[1284];
				bzero(msg_buf, sizeof(msg_buf));
				send(socket, msg_buf, sizeof(msg_buf), 0);
				return_ret = 0;
				break;
			}
			else if(username_cmp_ret == 0 && username_cmp_ret != 0)
			{
				struct msg rmsg = {-1, "smsg", "密码不正确"};
				char msg_buf[1284];
				bzero(msg_buf, sizeof(msg_buf));
				send(socket, msg_buf, sizeof(msg_buf), 0);
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

int sign_up(const char* data)
{
	struct si_data sid = parse_sign_in_data(data);
	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_insert_account, sid.username, sid.password);
	return exec_sql(fsql, NULL, NULL);
}

int sign_out(const char* data)
{
	char username[64];
	struct json_object* object = json_tokener_parse(data);
	struct json_object* obj_username_str = json_object_object_get(object, "username");
	strcpy(username, json_object_get_string(obj_username_str));

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_update_socket_by_username, -1, username);
	return exec_sql(fsql, NULL, NULL);
}