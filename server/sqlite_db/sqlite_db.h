#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

extern sqlite3 *create_db(void);                 // 创建数据库
extern void open(struct user_data *, sqlite3 *); // 注册账号,传账号和句柄
extern void exec(struct user_data *, sqlite3 *); // 登录账号,传账号和句柄
extern void close(sqlite3 *);                    // 关闭数据库

#endif
