#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//friend
static const char* sql_create_friend_table = "create table if not exists tb_friend(\
  id integer PRIMARY KEY autoincrement,\
  account_id integer,\
  friend_id integer,\
  create_at datetime default (datetime('now', 'localtime')),\
  update_at datetime default (datetime('now', 'localtime')),\
  FOREIGN KEY(account_id) REFERENCES tb_account(id),\
  FOREIGN KEY(friend_id) REFERENCES tb_account(id)\
),unique(id)";

static const char* sql_insert_friend = "insert into tb_friend(account_id,friend_id) values(%d,%d)";
static const char* sql_select_friends_by_accid =
	"select from tb_friend where account_id==%d or friend_id == %d";
static const char* sql_delete_friend_by_id = "delete from tb_friend where id==%d";

//friend msg
static const char* sql_create_fmsg_table = "create table if not exists tb_fmsg(\
  id integer PRIMARY KEY autoincrement,\
  send_id integer,\
  recv_id integer,\
  msg char[1024],\
  send_at datetime default (datetime('now','localtime')),\
  recv_at datetime default null,\
  FOREIGN KEY(send_id) REFERENCES tb_account(id),\
  FOREIGN KEY(recv_id) REFERENCES tb_account(id)\
)";

static const char* sql_add_fmsg = "insert to tb_fmsg values(%d,%d,'%s')";
static const char* sql_query_noorecv_fmsg_by_to =
	"select * from tb_fmsg where recv_id==%d and recv_at==null";

static const char* sql_update_fmsg_recv_nowtime_by_id =
	"update tb_fmsg set recv_time=(datetime('now','localtime')) where id=='%d'";
