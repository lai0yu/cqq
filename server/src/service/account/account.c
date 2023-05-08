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
	struct msg fmsg;
	memset(&fmsg, 0, sizeof(fmsg));
	fmsg.code = -1;

	struct si_data sid = parse_sign_in_data(data);

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_by_username, sid.username);

	struct db_row head;
	INIT_LIST_HEAD(&head.list);
	struct db_row* pos;

	int select_ret = exec_sql(fsql, select_common_callback, (void*)&head);
	if(select_ret == SQLITE_OK)
	{

		if(head.list.next != &head.list)
		{
			if(row_tick == 1)
			{
				int password_cmp = strcmp(sid.password, pos[0].argv[2]);
				if(password_cmp == 0)
				{
					char fsql[512];
					bzero(fsql, sizeof(fsql));
					sprintf(fsql, sql_update_socket_by_username, socket, sid.username);

					exec_sql(fsql, NULL, NULL);

					fmsg.code = 0;
					strcpy(fmsg.cmd, "fmsg");
					sprintf(fmsg.data, "用户名:%s 成功登陆", sid.username);

					const char* msg_buf = pack_msg(fmsg);
					send(socket, msg_buf, sizeof(fmsg), 0);
				}
				else
				{
					fmsg.code = -1;
					strcpy(fmsg.cmd, "fmsg");
					strcpy(fmsg.data, "密码错误, 请重试!");

					const char* msg_buf = pack_msg(fmsg);
					send(socket, msg_buf, sizeof(fmsg), 0);
				}
			}
			else
			{
				perror("数据库查询异常, 不应出现的多条数据");
			}
		}
		else
		{
			fmsg.code = -1;
			strcpy(fmsg.cmd, "fmsg");
			sprintf(fmsg.data, "用户名:%s 不存在", sid.username);

			const char* msg_buf = pack_msg(fmsg);
			send(socket, msg_buf, sizeof(fmsg), 0);
		}
	}
	else
	{
		perror("数据库查询异常, 不应出现的多条数据");
	}
	row_tick = 0;
	return fmsg.code;
}

int sign_up(const char* data, int socket)
{
	struct msg fmsg;
	memset(&fmsg, 0, sizeof(fmsg));
	fmsg.code = -1;

	struct si_data sid = parse_sign_in_data(data);

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_by_username, sid.username);

	struct db_row head;
	INIT_LIST_HEAD(&head.list);
	struct db_row* pos;

	int select_ret = exec_sql(fsql, select_common_callback, (void*)&head);

	if(select_ret == SQLITE_OK)
	{
		if(head.list.next != &head.list)
		{
			fmsg.code = -1;
			strcpy(fmsg.cmd, "fmsg");
			sprintf(fmsg.data, "用户名:%s已存在", sid.username);

			const char* msg_buf = pack_msg(fmsg);
			send(socket, msg_buf, sizeof(fmsg), 0);
			list_for_each_entry(pos, &head.list, list)
			{
				free(pos);
			}
		}
		else
		{

			bzero(fsql, sizeof(fsql));
			sprintf(fsql, sql_insert_account, sid.username, sid.password);
			int sql_ret = exec_sql(fsql, NULL, NULL);
			if(sql_ret == SQLITE_OK)
			{
				fmsg.code = 0;
				strcpy(fmsg.cmd, "fmsg");
				sprintf(fmsg.data, "用户：%s 注册成功", sid.username);
				const char* msg_buf = pack_msg(fmsg);
				send(socket, msg_buf, sizeof(fmsg), 0);
			}
		}
	}
	else
	{
		perror("数据库查询异常");
	}
	row_tick = 0;
	return fmsg.code;
}

int sign_out(const char* data, int socket)
{
	struct msg fmsg;
	memset(&fmsg, 0, sizeof(fmsg));
	fmsg.code = -1;

	char username[64];
	struct json_object* object = json_tokener_parse(data);
	struct json_object* obj_username_str = json_object_object_get(object, "username");
	strcpy(username, json_object_get_string(obj_username_str));

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_update_socket_by_username, -1, username);

	int update_ret = exec_sql(fsql, NULL, NULL);

	if(update_ret == SQLITE_OK)
	{
		fmsg.code = 0;
		strcpy(fmsg.cmd, "fmsg");
		strcpy(fmsg.data, "成功登出!");

		const char* msg_buf = pack_msg(fmsg);
		send(socket, msg_buf, sizeof(fmsg), 0);
	}
	return fmsg.code;
}