#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lib/json_util/json_util.h"

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

#define PULL_FRIENDS_LIST 41          // 拉取好友列表
#define NO_FRIENDS 42
#define LISTING_FRIEND 43             // 一条一条发
#define LISTING_FRIEND_FINISHED 45    // 好友列表发送完成

#define ADD_FRIEND 51                 // 请求添加好友
#define ADD_FRIEND_REQUEST 52         // 受到添加好友的请求
#define ADD_FRIEND_NO_USER 53         // 请求添加好友无此用户
#define ADD_FRIEND_PENDING 54         // 请求添加好友等待同意
#define ADD_FRIEND_ADMIT 55           // 同意添加好友
#define ADD_FRIEND_REFUSE 56          // 拒绝添加好友
#define ADD_FRIEND_DUP 57             // 已经是好友
#define ADD_FRIEND_BLACK 58           // 已经拉黑对方和被对方拉黑

#define SEND_MSG_TO_FRIEND 70         // 给好友发送消息
#define SEND_MSG_NO_USER 71           // 未知好友名
#define SEND_MSG_TO_FRIEND_REQUEST 75 // 客户端被服务器接收消息
#define SEND_MSG_OFFLINE 72           // 消息被离线
#define SEND_MSG_RECV 73              // 消息被接收

#define PULL_OFFLINE_MSG 80           // 拉取所有离线消息
#define LISTING_OFFLINE_MSG 81        // 一条一条发
#define LISTING_OFFLINE_FINISH 82     // 离线消息发送

int serv_sock;
struct msg cmsg;
struct sign_data sdata;

char send_buf[1024] = {0};
int send_ret = -1;

char recv_buf[1024];
int recv_ret = -1;

void login_ui();
void main_ui();

void fun_login();
void fun_main();

void fun_sign_in();
void fun_sign_up();
void fun_sign_out();

void fun_friend_list();
void fun_friend_add();
void fun_friend_del();
void fun_friend_black();

void sign_in_return(struct msg smsg);
void sign_out_return(struct msg smsg);
void sign_up_return(struct msg smsg);
void sign_del_return(struct msg smsg);

void fun_friend();
void fun_group();

void *recv_work(void *argv) {
    recv_ret = recv(serv_sock, recv_buf, sizeof(recv_buf), 0);
    if (recv_ret == 0) {
        printf("服务器可能已经宕机,关闭连接\n");
        close(serv_sock);
        return recv_buf;
    } else {
        cmsg = parse_msg(recv_buf);
        switch (cmsg.code) {
        case 0 ... 9: sign_in_return(cmsg); break;
        case 10 ... 19: break;
        case 20 ... 29: sign_up_return(cmsg); break;
        case 30 ... 39: break;
        case 40 ... 49: break;
        case 50 ... 59: break;
        case 60 ... 69: break;
        case 70 ... 79: break;
        case 80 ... 89: break;
        case 90 ... 99: break;
        case 100 ... 109: break;
        case 110 ... 119: break;
        case 120 ... 129: break;
        case 130 ... 139: break;

        default: break;
        }
    }
    return NULL;
}

// 向服务器发送消息
static int send_msg(int socket, char code, char *data) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(struct msg));
    smsg.code = code;
    strcpy(smsg.data, data);

    bzero(send_buf, sizeof(send_buf));
    pack_msg(smsg, send_buf);
    send_ret = send(socket, send_buf, strlen(send_buf) + 1, 0);
    return send_ret;
}

// ui界面
void login_ui() {
    printf("***************登录界面*****************\n");
    printf("*                                     *\n");
    printf("*              1:登录                  *\n");
    printf("*                                     *\n");
    printf("*              2:注册                  *\n");
    printf("*                                     *\n");
    printf("*              3:退出系统              *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void main_ui() {
    printf("***************功能界面*****************\n");
    printf("*                                     *\n");
    printf("*              1:好友功能              *\n");
    printf("*                                     *\n");
    printf("*              2:群聊列表              *\n");
    printf("*                                     *\n");
    printf("*              3:退出登录              *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void friend_ui() {
    printf("***************好友界面*****************\n");
    printf("*                                     *\n");
    printf("*              1:好友列表              *\n");
    printf("*                                     *\n");
    printf("*              2:加好友                *\n");
    printf("*                                     *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void fun_friend() {
    friend_ui();
    printf("请输入选项:\n");
    char c;
    scanf("%c", &c);
    switch (c) {
    case '1': fun_friend_list(); break;
    case '2': fun_friend_add(); break;
    default:
        system("clear");
        printf("选项不正确！请重新输入\n");
        fun_login();
        break;
    }
}

void fun_login() {
    login_ui();
    printf("请输入选项:\n");
    char c;
    scanf("%c", &c);
    switch (c) {
    case '1': fun_sign_in(); break;
    case '2': fun_sign_up(); break;
    case '3': exit(0); break;
    default:
        system("clear");
        printf("选项不正确！请重新输入\n");
        fun_login();
        break;
    }
}

void fun_main() {
    main_ui();
    printf("请输入选项:\n");
    char c;
    scanf("%c", &c);
    switch (c) {
    case '1': fun_friend(); break;
    case '2': fun_group(); break;
    case '3': fun_sign_out(); break;
    default:
        system("clear");
        printf("选项不正确！请重新输入\n");
        fun_main();
        break;
    }
}

void fun_sign_in() {
    printf("用户名：\n");
    scanf("%s", sdata.username);
    printf("密码：\n");
    scanf("%s", sdata.password);
    char msg[1020];
    pack_signdata(sdata, msg);
    send_msg(serv_sock, SIGN_IN, msg);
    if (send_msg > 0) {
        recv_work(NULL);
    }
}

void sign_in_return(struct msg smsg) {
    switch (smsg.code) {
    case SIGN_IN_SUCCESS:
        system("clear");
        printf("登录成功，跳转到主页面\n");
        fun_main();
        break;
    case SIGN_IN_NO_USER:
        system("clear");
        printf("没有此用户名，请重新输入\n");
        fun_sign_in();
        break;
    case SIGN_IN_PW_ERROR:
        system("clear");
        printf("密码错误\n");
        fun_sign_in();
        break;

    default:
        break;
    }
}

void fun_sign_up() {
    printf("用户名：\n");
    struct sign_data sid;
    scanf("%s", sid.username);
    printf("密码：\n");
    scanf("%s", sid.password);

    char repassword[64];
    printf("重复密码：\n");
    scanf("%s", repassword);

    if (strcmp(sid.password, repassword) == 0) {
        char msg[1020];
        pack_signdata(sid, msg);
        send_msg(serv_sock, SIGN_UP, msg);
        if (send_ret > 0) {
            recv_work(NULL);
        }
    } else {
        system("clear");
        printf("重复密码错误,请重新输入\n");
        fun_sign_up();
    }
}

void sign_up_return(struct msg smsg) {
    switch (smsg.code) {
    case SIGN_UP_DUP_USER:
        system("clear");
        printf("用户名已经被占用,请重新输入\n");
        fun_sign_up();
        break;
    case SIGN_UP_SUCCESS:
        system("clear");
        printf("注册成功,跳转到登录界面\n");
        fun_login();
        break;
    }
}

void fun_sign_out() {
    char msg[1020];
    pack_signdata(sdata, msg);
    send_msg(serv_sock, SIGN_OUT, msg);
    if (send_msg > 0) {
        recv_work(NULL);
    }
    system("clear");
    printf("已经退出到登录页面");
    fun_login();
}

void fun_friend_list() {
}
void fun_friend_add() {
    struct add_friend_data afd;
    strcpy(afd.username, sdata.username);
    printf("好友名：\n");
    scanf("%s", afd.friendname);
    char msg[512];
    pack_afdata(afd, msg);
    send_msg(serv_sock, ADD_FRIEND, msg);
    if (send_msg > 0) {
        recv_work(NULL);
    }
}
void fun_friend_del() {
}

int main(int argc, char *argv[]) {
    // 默认服务器IP 端口
    char serv_ip[128] = "192.168.60.91";
    char serv_port[128] = "54321";

    if (argc == 3) {
        bzero(serv_ip, sizeof(serv_ip));
        bzero(serv_port, sizeof(serv_port));
        strcpy(serv_ip, argv[1]);
        strcpy(serv_port, argv[2]);
    }
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    struct sockaddr_in serv_sock_addr;
    serv_sock_addr.sin_family = AF_INET;
    serv_sock_addr.sin_addr.s_addr = inet_addr(serv_ip);
    serv_sock_addr.sin_port = htons(atoi(serv_port));

    int connect_ret = connect(serv_sock, (struct sockaddr *)&serv_sock_addr, sizeof(serv_sock_addr));
    if (connect_ret < 0) {
        perror("connect error");
        return -1;
    }

    system("clear");
    printf("欢迎进入到真爱qq系统\n");
    fun_login();

    close(serv_sock);
    serv_sock = -1;
    return 0;
}
