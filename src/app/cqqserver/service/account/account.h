#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "../../../../lib/linux_list/linux_list.h"
#include "../../../../lib/sqlite_util/sqlite_util.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* sql_create_account_table = "create table if not exists account(\
  id integer PRIMARY KEY autoincrement,\
  username char[64],\
  password char[64],\
  create_at datetime default (datetime('now', 'localtime')),\
  update_at datetime default (datetime('now', 'localtime'))\
),unique(id,username)";
static const char* sql_insert_account = "insert to account values('%s','%s')";
static const char* sql_select_by_username = "select * from account where username=='%s'";
static const char* sql_update_username_by_id = "update account set username='%s' where id=%d";
static const char* sql_update_password_by_id = "update account set password='%s' where id=%d";

extern int init_service();
extern int sign_in(const char* username, const char* password);
extern int sign_up(const char* username, const char* password);
#endif