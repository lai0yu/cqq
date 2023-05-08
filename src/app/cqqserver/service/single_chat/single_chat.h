#include "../../../../lib/linux_list/linux_list.h"
#include "../../../../lib/sqlite_util/sqlite_util.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//friend
static const char* sql_create_friend_table = "create table if not exists tb_friend(\
  id integer PRIMARY KEY autoincrement,\
  account_id integer\
  friend_id integer,\
  create_at datetime default (datetime('now', 'localtime')),\
  update_at datetime default (datetime('now', 'localtime')),\
  FOREIGN KEY(account_id) REFERENCES tb_account(id),\
  FOREIGN KEY(friend_id) REFERENCES tb_account(id)\
),unique(id)";

static const char* sql_add_friend = "insert to tb_friend values(%d,%d)";
static const char* sql_query_friends_by_rel =
	"select * from tb_friend where account_id==%d or friend_id==%d";
static const char* sql_query_friend_by_id = "delete  from tb_friend where id==%d";
static const char* sql_delete_friend_by_rel =
	"delete from tb_friend where account_id==%d && friend_id==%d";
static const char* sql_delete_friend_by_id = "delete from tb_friend where id==%d";

//friend msg

static const char* sql_create_fmsg_table = "create talble if not exists tb_fmsg(\
  id integer PRIMARY KEY autoincrement,\
  send_id integer,\
  recv_id integer,\
  msg char[1024],\
  send_at datetime default (datetime('now','localtime')),\
  recv_at datetime default null,\
  FOREIGN KEY(from_id) REFERENCES tb_account(id),\
  FOREIGN KEY(to_id) REFERENCES tb_account(id)\
)";

static const char* sql_add_fmsg = "insert to tb_fmsg values(%d,%d,%s)";
static const char* sql_query_noorecv_fmsg_by_to =
	"select * from tb_fmsg where recv_id==%d and recv_at==null";

static cosnt char* sql_update_fmsg_recvtime_by_id = "update tb_fmsg set r"
