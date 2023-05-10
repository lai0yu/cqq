#ifndef __MSG_CODE__
#define __MSG_CODE__

//消息code
#define UNKNOW_ERROR -2 //服务器返回错误，如果可以，返回逻辑错误
#define ERROR -1 //服务器返回错误 只是业务级别
#define OK 0 //服务器返回正确

#define PENDING 1 //待处理，

#define SIGN_UP 2 //账户注册  服务器应该返回OK或者ERROR
#define SING_DEL 3 //账户删除  服务器应该返回OK或者ERROR

#define SIGN_IN 4 //账户登录 服务器应该返回OK或者ERROR
#define SING_OUT 5 //账户登出 服务器应该返回OK或者ERROR

#define ADD_FRIEND 6 //申请加好友, 服务器应该返回OK或者PENDING
#define ADD_FRIEND_ADMIT 7 //同意加好友, 服务器应该返回OK或者ERROR

#define DEL_FRIEND 8 //删除好友 服务器应该返回OK或者PENDING
#define BLACK_FRIEND 9 //拉黑好友 服务器应该返回OK或者PENDING

#define LISTING 10 // 列表中
#define LIST_FRIENDS 11 //请求好友列表，服务器按条发送，code为LISTING，最后再一个发送OK表示列表完毕

//发送单聊消息，服务器应该返回OK(成功)，PENDING(待处理表示离线消息), 失败ERROR
#define SEND_SINGLE_MSG 12

#define READ_SINGLE_MSG 13 //确认接收到消息,服务器应该返回OK(成功),失败ERROR

//请求未读消息，服务器按条发送，code为LISTING，最后再一个发送OK表示列表完毕
#define LIST_ALL_UNREAD 14

#define CREATE_GROUP 15

#define ADD_GROUP_MEMBER 111
#define DEL_GROUP_MEMBER 111

#define LIST_GROUP_MEMBER 111

#define SEND_GROUP_MSG 16
#define READ_GROUP_MSG 17

#define BAN_GROUP_MEMBER 123

#define READ_GROUP_MSG 17

#endif