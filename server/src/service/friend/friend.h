#ifndef __FRIEND_H__
#define __FRIEND_H__

#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sys/socket.h>

#define PULL_FRIENDS_LIST 41        // 拉取好友列表
#define NO_FRIENDS 42
#define LISTING_FRIEND 43           // 一条一条发
#define LISTING_FRIEND_FINISHED 45  // 好友列表发送完成

#define ADD_FRIEND 51               // 请求添加好友
#define ADD_FRIEND_REQUEST 52       // 受到添加好友的请求
#define ADD_FRIEND_NO_USER 53       // 请求添加好友无此用户
#define ADD_FRIEND_PENDING 54       // 请求添加好友等待同意
#define ADD_FRIEND_ADMIT 55         // 同意添加好友
#define ADD_FRIEND_REFUSE 56        // 拒绝添加好友
#define ADD_FRIEND_DUP 57           // 已经是好友
#define ADD_FRIEND_BLACK 58         // 已经拉黑对方和被对方拉黑

#define DEL_FRIEND 55123            // 删除好友
#define DEL_FRIEND_SUCCESS 56123    // 删除好友成功
#define DEL_FRIEND_ERROR 123123     // 删除好友失败

#define BLACK_USER 60123            // 拉黑
#define BLACK_USER_SUCESS 611231232 // 拉黑成功
#define BLACK_USER_ERROR 6223234    // 拉黑失败

/*
*    A                    B
* username    0      friendname   没有关系
*
* username    1      friendname   A已经向B申请好友，等待B同意
* username    2      friendname   B已经同意A的好友申请
*
* username    3      friendname   B已经向A申请好友，等待A同意
* username    4      friendname   A已经同意B的好友申请

* username    5      friendname   A已经拉黑B
* username    7      friendname   B已经拉黑A
*
*/

static const char *sql_create_friend_table =
    "create table if not exists tb_friend(\
  username char[64],\
  friendname char[64],\
  relation integer default 0,\
  primary key(username,friendname)\
)";

extern int init_friend_service();

extern int list_friends(const char *data, int socket);
extern int add_friend(const char *data, int socket);
extern int add_friend_admit(const char *data, int socket);
extern int add_friend_refuse(const char *data, int socket);
extern int del_friend(const char *data, int socket);
extern int black_friend(const char *data, int socket);
#endif
