#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

//friend
static const char* sql_create_friend_table = "create table if not exists tb_friend(\
  id integer PRIMARY KEY autoincrement,\
  account_id integer,\
  friend_id integer,\
  create_at datetime default (datetime('now', 'localtime')),\
  update_at datetime default (datetime('now', 'localtime'))\
)";

static const char* sql_insert_friend = "insert into tb_friend(account_id,friend_id) values(%d,%d)";
static const char* sql_select_friends_by_accid =
	"select from tb_friend where account_id==%d or friend_id == %d";
static const char* sql_delete_friend_by_id = "delete from tb_friend where id==%d";

static const char* sql_select_account_by_id = "select * from tb_account where id==%d";

static const char* sql_select_account_by_username = "select * from tb_account where username=='%s'";

//friend msg
static const char* sql_create_fmsg_table = "create table if not exists tb_fmsg(\
  id integer PRIMARY KEY autoincrement,\
  send_id integer,\
  recv_id integer,\
  msg char[1284],\
  send_at datetime default (datetime('now','localtime')),\
  recv_at datetime default null\
)";

static const char* sql_insert_fmsg = "insert to tb_fmsg(send_id,recv_id,msg) values(%d,%d,'%s')";
static const char* sql_insert_noorecv_fmsg_by_to =
	"select * from tb_fmsg where recv_id==%d and recv_at==null";

static const char* sql_update_fmsg_recv_nowtime_by_id =
	"update tb_fmsg set recv_time=(datetime('now','localtime')) where id=='%d'";

extern int init_singlechat_service();

extern int get_friend_list(const char* username);
