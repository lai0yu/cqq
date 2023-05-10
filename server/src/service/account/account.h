#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static const char* sql_create_account_table = "create table if not exists tb_account(\
    username char [64] PRIMARY KEY,\
    password char [64],\
    socket integer default -1,\
    create_at datetime default (datetime('now', 'localtime')),\
    update_at datetime default (datetime('now', 'localtime'))\
)";

#define SIGN_IN 1
#define SIGN_IN_SUCCESS 2
#define SIGN_IN_NO_USER 3
#define SIGN_IN_PW_ERROR 4

#define SIGN_OUT 11
#define SIGN_OUT_SUCCESS 12

#define SIGN_UP 21
#define SIGN_UP_SUCCESS 22
#define SIGN_UP_DUP_USER 23

#define SIGN_DEL 31
#define SIGN_DEL_SUCCESS 32
#define SIGN_DEL_NO_USER 33
#define SIGN_DEL_PW_ERROR 34

extern int init_account_service();
extern int sign_in(const char* data, int socket);
extern int sign_del(const char* data, int socket);
extern int sign_up(const char* data, int socket);
extern int sign_out(const char* data, int socket);

#endif