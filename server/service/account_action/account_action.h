#ifndef __ACCOUNT_ACTION_H__
#define __ACCOUNT_ACTION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
// 创建数据库
extern void reg_user(struct user_data *, sqlite3 *);   // 注册账号,传账号和句柄
extern void login_user(struct user_data *, sqlite3 *); // 登录账号,传账号和句柄
extern void close(sqlite3 *);                          // 关闭数据库

#endif