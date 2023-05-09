#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static const char* sql_create_account_table = "create table if not exists tb_account(\
  id integer PRIMARY KEY autoincrement,\
  username char[64] UNIQUE,\
  password char[64],\
  socket integer default -1,\
  create_at datetime default (datetime('now', 'localtime')),\
  update_at datetime default (datetime('now', 'localtime'))\
)";

static const char* sql_insert_account =
	"insert into tb_account(username,password) values('%s','%s')";
static const char* sql_select_by_username = "select * from tb_account where username=='%s'";
static const char* sql_update_username_by_id = "update tb_account set username='%s' where id=%d";
static const char* sql_update_password_by_id = "update tb_account set password='%s' where id=%d";
static const char* sql_delete_by_username = "delete from tb_account where username='%s'";
static const char* sql_update_socket_by_username =
	"update tb_account set socket=%d where username='%s'";

struct si_data
{
	char username[64];
	char password[64];
};

static inline struct si_data parse_sign_in_data(const char* json_str)
{
	struct si_data pack;

	memset(&pack, 0, sizeof(pack));

	struct json_object* object = json_tokener_parse(json_str);

	struct json_object* obj_username_str = json_object_object_get(object, "username");
	struct json_object* obj_password_str = json_object_object_get(object, "password");

	strcpy(pack.username, json_object_get_string(obj_username_str));
	strcpy(pack.password, json_object_get_string(obj_password_str));

	return pack;
}

extern int init_account_service();
extern int sign_in(const char* data, int socket);
extern int sign_up(const char* data, int socket);
extern int sign_out(const char* data, int socket);

#endif