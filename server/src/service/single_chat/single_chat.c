#include "single_chat.h"

int init_singlechat_service()
{

	exec_sql(sql_create_friend_table, NULL, NULL);
	exec_sql(sql_create_fmsg_table, NULL, NULL);
	return 0;
}

int get_friend_list(const char* username)
{

	char fsql[512];
	bzero(fsql, sizeof(fsql));
	sprintf(fsql, sql_select_account_id_by_username, username);

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
