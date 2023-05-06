#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

extern sqlite3 *open_db(void); //打开数据库,返回句柄
extern void close(sqlite3 *); //关闭数据库,传进句柄
extern void exec_add(char *,sqlite3 *); //添加数据
extern void exec_del(char *,sqlite3 *); //删除数据
extern void exec_update(char *,sqlite3 *); //修改数据
extern void exec_select(char *,sqlite3 *,void *); //查询数据

#endif
