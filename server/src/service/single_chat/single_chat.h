#include "../../lib/json_util/json_util.h"
#include "../../lib/sqlite_util/sqlite_util.h"
#include <sys/socket.h>

#define PULL_FRIENDS 41             //拉取好友列表
#define NO_FRIENDS 42
#define LISTING_FRIEND 43           //一条一条发
#define LISTING_FRIEND_FINISHED 45 //好友列表发送完成


#define ADD_FRIEND    51       //请求添加好友
#define ADD_FRIEND_NO_USER 41  //请求添加好友无此用户
#define ADD_FRIEND_PENDING 42  //请求添加好友等待同意
#define ADD_FRIEND_ADMIT 43    //同意添加好友
#define ADD_FRIEND_REFUSE 44   //拒绝添加好友

#define DEL_FRIEND   55        // 删除好友
#define DEL_FRIEND_SUCCESS 56  // 删除好友成功
#define DEL_FRIEND_ERROR       // 删除好友失败

#define BLACK_USER        60   //拉黑
#define BLACK_USER_SUCESS 61   //拉黑
#define BLACK_USER_ERROR  62   //拉黑

#define SEND_MSG_TO_FRIEND 0    //给好友发送消息
#define MSG_OFFLINE    1    //消息被离线
#define MSG_RECV     2    //消息被接收

#define PULL_OFFLINE_MSG   0 //拉取所有离线消息
#define LISTING_OFFLINE_MSG           //一条一条发
#define LISTING_LISTING_OFFLINE_FINISHED  //好友列表发送完成



static const char* sql_create_friend_table = "create table if not exists tb_friend(\
  username char[64],\
  friendname char[64],\
  relation integer default 0,\
  primary key(username,friendname)\
)";

static const char* sql_create_fmsg_table = "create table if not exists tb_fmsg(\
  id integer primary key autoincrement,\
  send_user char[64],\
  recv_user char[64],\
  code char,\
  data char[1023],\
  send_at datetime default (datetime('now','localtime')),\
  recv_at datetime default null\
)";
extern int init_single_chat_service();

extern int list_friends(const char* data,int socket);
extern int add_friends(const char* data, int socket);

