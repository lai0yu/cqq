#include "account.h"

int init_account_service() { return exec_sql(sql_create_account_table, NULL, NULL); }

static int send_msg(int socket, int code, char* data) {
	struct msg smsg;
	memset(&smsg, 0, sizeof(struct msg));
	smsg.code = code;
	strcpy(smsg.data, data);

	char msg_buf[1024] = { 0 };
	pack_msg(smsg, msg_buf);
	return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

int sign_up(const char* data, int socket) {
	struct sign_data sd = parse_signdata(data);

	char where_buf[256] = { 0 };
	sprintf(where_buf, "username=='%s'", sd.username);
	struct select_row row_head = db_select("tb_account", "(password,socket)", where_buf);

	int count = list_entry(row_head.list.prev, struct select_row, list)->index;

	if (count < 1) {
		send_msg(socket, SIGN_IN_NO_USER, "无此用户");
	} else {
		char values_buf[256] = { 0 };
		sprintf(values_buf, "('%s','%s')", sd.username, sd.password);
		db_insert("tb_account(username,password)", values_buf);
		send_msg(socket, SIGN_IN_SUCCESS, "注册成功");
	}
	free_select_rows_list(&row_head);
	return 0;
}

int sign_del(const char* data, int socket) {

	struct sign_data sd = parse_signdata(data);

	char where_buf[256] = { 0 };
	sprintf(where_buf, "username=='%s", sd.username);
	struct select_row row_head = db_select("tb_account", "(password,socket)", where_buf);

	int count = list_entry(row_head.list.prev, struct select_row, list)->index;
	if (count < 1) {
		send_msg(socket, SIGN_DEL_NO_USER, "无此用户");
	} else {
		char* db_password = list_entry(row_head.list.next, struct select_row, list)->argv[0];
		int cmp = strcmp(db_password, sd.username);
		if (cmp == 0) {
			char where_buf[256] = { 0 };
			sprintf(where_buf, "username=='%s'", sd.username);
			db_delete("tb_account", where_buf);
			send_msg(socket, SIGN_DEL_SUCCESS, "注销账户成功");
		} else {
			send_msg(socket, SIGN_DEL_PW_ERROR, "密码错误");
		}
	}
	free_select_rows_list(&row_head);
	return 0;
}

int sign_in(const char* data, int socket) {
	struct sign_data sd = parse_signdata(data);

	char where_buf[256] = { 0 };
	sprintf(where_buf, "username=='%s'", sd.username);
	struct select_row row_head = db_select("tb_account", "(password,socket)", where_buf);

	int count = list_entry(row_head.list.prev, struct select_row, list)->index;

	if (count < 1) {
		send_msg(socket, SIGN_IN_NO_USER, "无此用户");
	} else {
		char* db_password = list_entry(row_head.list.next, struct select_row, list)->argv[0];
		int cmp = strcmp(db_password, sd.username);
		if (cmp == 0) {

			char sets_buf[256] = { 0 }, where_buf[256] = { 0 };
			sprintf(sets_buf, "socket=%d", socket);
			sprintf(where_buf, "username=='%s'", sd.username);
			db_update("tb_account", sets_buf, where_buf);

			send_msg(socket, SIGN_IN_SUCCESS, "登录成功");
		} else {
			send_msg(socket, SIGN_IN_PW_ERROR, "密码错误");
		}
	}
	free_select_rows_list(&row_head);
	return 0;
}

int sign_out(const char* data, int socket) {
	struct sign_data sd = parse_signdata(data);
	char where_buf[256] = { 0 };
	sprintf(where_buf, "username=='%s", sd.username);
	db_update("tb_account", "set socket=-1", where_buf);
	send_msg(socket, SIGN_OUT_SUCCESS, "登出成功");
	return 0;
}
