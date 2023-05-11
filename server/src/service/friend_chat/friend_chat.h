#ifndef __FRIEND_CHAT_H__
#define __FRIEND_CHAT_H__

#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sys/socket.h>
#include <time.h>

#define SEND_MSG_TO_FRIEND 70         // 给好友发送消息
#define SEND_MSG_NO_USER 71           // 未知好友名
#define SEND_MSG_TO_FRIEND_REQUEST 75 // 客户端被服务器接收消息
#define SEND_MSG_OFFLINE 72           // 消息被离线
#define SEND_MSG_RECV 73              // 消息被接收

#define PULL_OFFLINE_MSG 80           // 拉取所有离线消息
#define LISTING_OFFLINE_MSG 81        // 一条一条发
#define LISTING_OFFLINE_FINISH 82     // 离线消息发送

static const char *sql_create_fmsg_table = "create table if not exists tb_fmsg(\
  uuid integer primary key,\
  send_name char[64],\
  recv_name char[64],\
  code char,\
  data char[1023],\
  send_at datetime default (datetime('now','localtime')),\
  recv_at datetime default null\
)";

extern int init_friend_chat_service();

extern int send_msg_to_friend(const char *data, int socket);
extern int recv_friend_msg(const char *data, int socket);
extern int list_offline_msg(const char *data, int socket);

#endif