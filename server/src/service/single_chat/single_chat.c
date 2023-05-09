#include "single_chat.h"

int init_singlechat_service()
{

	exec_sql(sql_create_friend_table, NULL, NULL);
	exec_sql(sql_create_fmsg_table, NULL, NULL);
	return 0;
}

int add_friend(const char* data)
{
	//parse data
	char username[64], friendname[64];
	struct json_object* object = json_tokener_parse(data);
	struct json_object* obj_username_str = json_object_object_get(object, "username");
	struct json_object* obj_friendame_str = json_object_object_get(object, "friendname");
	strcpy(username, json_object_get_string(obj_username_str));
	strcpy(friendname, json_object_get_string(obj_friendame_str));

	//select account info by username
	struct db_row srow;
	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_account_by_username, username);
	exec_sql(fsql, select_single_callback, (void*)&srow);
	int self_account_id = atoi(srow.argv[0]);

	//select account info by friend name
	struct db_row frow;
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_account_by_username, friendname);
	exec_sql(fsql, select_single_callback, (void*)&frow);
	int friend_sock = atoi(frow.argv[3]);
	int friend_account_id = atoi(frow.argv[0]);

	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_insert_fmsg, self_account_id, friend_account_id);
	exec_sql(fsql, NULL, NULL);

	if(friend_sock > 0)
	{
	}

	struct msg server_msg;
	memset(&server_msg, 0, sizeof(server_msg));

	server_msg.code = 1;
	strcpy(server_msg.cmd, "server_msg");

	char send_data[1024] = "申请已发送，对方同意后会出现在你的好友列表中";
}

int get_friend_list(const char* username)
{

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_account_by_username, username);

	//get account id by username
	struct db_row urow;
	memset(&urow, 0, sizeof(urow));
	int select_ret = exec_sql(fsql, select_single_callback, (void*)&urow);
	int account_id = atoi(urow.argv[0]);

	//get all friend
	struct db_row friend_list;
	INIT_LIST_HEAD(&friend_list.list);
	struct db_row* pos;

	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_friends_by_accid, account_id);
	select_ret = exec_sql(fsql, select_common_callback, (void*)&friend_list);

	//use foreach send friends list to user
	char send_buf[1284];
	int csock = atoi(urow.argv[3]);
	struct db_row frow;

	struct msg fmsg;
	memset(&fmsg, 0, sizeof(fmsg));
	fmsg.code = -1;
	int s_id, f_id, f_sock;
	list_for_each_entry(pos, &friend_list.list, list)
	{
		//select usename by account_id;
		s_id = atoi(pos->argv[1]);
		f_id = atoi(pos->argv[2]);

		memset(&frow, 0, sizeof(frow));
		bzero(fsql, sizeof(fsql));
		sprintf(fsql, sql_select_account_by_id, account_id == s_id ? f_id : s_id);
		exec_sql(fsql, select_single_callback, (void*)&frow);

		f_sock = atoi(frow.argv[3]);

		memset(&fmsg, 0, sizeof(fmsg));
		fmsg.code = -1;
		strcpy(fmsg.cmd, "friend_list_element");
		char send_data[1024];
		bzero(send_data, sizeof(send_data));
		sprintf(send_data, "好友：%s,(%s)", frow.argv[1], f_sock > 0 ? "在线" : "离线");

		bzero(send_buf, sizeof(send_buf));
		strcpy(send_buf, pack_msg(fmsg));

		send(csock, send_buf, sizeof(send_buf), 0);
	}
	//TODO:free urow;

	//TODO:free friendlist;

	//TODO:free frow;

	return 0;
}
