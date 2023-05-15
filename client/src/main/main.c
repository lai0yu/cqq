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

static int serv_sock;
char *data[512];
char *on[512];
int count = 0;

static char gusername[64];
static char gpassword[64];

int sign_in();
int sign_up();

void login_ui();
void main_ui();
void friend_ui(char *, char *, int);
void message_ui(char *, char *);

void fun_login();
void fun_main();
void friend_main();

void friend_char(char *);

// ui界面
void login_ui() {
    printf("***************登录界面*****************\n");
    printf("*                                     *\n");
    printf("*              1:登录                  *\n");
    printf("*                                     *\n");
    printf("*              2:注册                  *\n");
    printf("*                                     *\n");
    printf("*              3:退出系统             *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void main_ui() {
    printf("***************功能界面*****************\n");
    printf("*                                     *\n");
    printf("*              1:好友列表              *\n");
    printf("*                                     *\n");
    printf("*              2:群列表                *\n");
    printf("*                                     *\n");
    printf("*              3:添加好友              *\n");
    printf("*                                     *\n");
    printf("*              4:删除好友              *\n");
    printf("*                                     *\n");
    printf("*              5:拉黑好友              *\n");
    printf("*                                     *\n");
    printf("*              6:创建群聊              *\n");
    printf("*                                     *\n");
    printf("*              7:删除群聊              *\n");
    printf("*                                     *\n");
    printf("*              8:退出登录              *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void friend_ui(char *data, char *on, int count) {
    int i;
    printf("***************好友列表*****************\n");
    printf("*                                     *\n");
    for (i = 0; i < count; i++) {
        printf("            ++i:%s(%s)             \n", data[i], on[i]);
        printf("*                                     *\n");
    }
    printf("*           q:返回功能界面           *\n");
    printf("*                                     *\n");
    printf("***************************************\n");
}

void message_ui(char *friendname, char *online) {
    printf("**************%s的聊天窗口(%s)*****************\n", friendname, online);
    printf("*                                     *\n");
    printf("*              1:未未未未              *\n");
    printf("*                                     *\n");
    printf("*              2:未未未未              *\n");
    printf("*                                     *\n");
    printf("*              3:未未未未              *\n");
    printf("*                                     *\n");
    printf("*              4:未未未未              *\n");
    printf("*                                     *\n");
    printf("****************************************************\n");
}

// 向服务器发送消息
static int send_msg(int socket, char code, char *data) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(struct msg));
    smsg.code = code;
    strcpy(smsg.data, data);

    char msg_buf[1024] = {0};
    pack_msg(smsg, msg_buf);
    return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

// 登录模块
int sign(char code) {
    struct sign_data sd;
    memset(&sd, 0, sizeof(struct sign_data));

    printf("请输入用户名:\n");
    scanf("%s", sd.username);

    printf("请输入密码:\n");
    scanf("%s", sd.password);

    char psd[1023];
    pack_signdata(sd, psd);
    return send_msg(serv_sock, code, psd);
}
// 登录输入
void fun_login(void) {
    login_ui();
    unsigned char c;
    printf("请输入:\n");
    scanf("%hhd", &c);
    if (c < 1 || c > 3) {
        printf("输入有误,请重新输入\n");
        scanf("%hhd", &c);
    }
    switch (c) {
    case 1:
        sign(SIGN_IN);
        break;
    case 2:
        sign(SIGN_UP);
        break;
    case 3:
        exit(-1);
    }
}

// 功能输入
void fun_main() {
    main_ui();
    unsigned char c;
    printf("请输入:\n");
    scanf("%hhd", &c);
    if (c < 1 || c > 8) {
        printf("输入有误,请重新输入\n");
        scanf("%hhd", &c);
    }
    switch (c) {
    case 1:
        send_msg(serv_sock, PULL_FRIENDS_LIST, "拉取好友列表");
        break;
    case 2:
        // send_msg(serv_sock,XXXX,"拉取群聊列表");
        break;
    case 3: {
        struct add_friend_data *afd = (struct add_friend_data *)malloc(sizeof(struct add_friend_data));
        bzero(afd, sizeof(struct add_friend_data));
        char buf[100];
        printf("请输入您的用户名:");
        scanf("%s", afd->username);
        printf("请输入您的好友名:");
        scanf("%s", afd->friendname);
        bzero(buf, sizeof(buf));
        pack_afdata(*afd, buf);
        send_msg(serv_sock, ADD_FRIEND, buf);
        break;
    }
    case 4:
        // send_msg(serv_sock,DEL_FRIEND,"请求删除好友");
        break;
    case 5:
        // send_msg(serv_sock,BLACK_USER,"请求拉黑好友");
        break;
    case 6:
        // send_msg(serv_sock,XXXX,"请求创建群聊");
        break;
    case 7:
        // send_msg(serv_sock,XXXX,"请求删除群聊");
        break;
    case 8:
        send_msg(serv_sock, SIGN_OUT, "请求退出登录");
        break;
    }
}

// 好友列表功能
void friend_main() {
    friend_ui(data, on, count);
    int c;
    printf("请输入:\n");
    scanf("%d", &c);
    if (c < 1 || c > count) {
        printf("输入有误,请重新输入\n");
        scanf("%d", &c);
    }
    char *friendname[64];
    char *online[30];
    if (c == 113) {
        fun_main();
    } else {
        bzero(friendname, sizeof(char *) * 64);
        bzero(online, sizeof(char *) * 30);
        strcpy(friendname, data[c]);
        strcmp(on[c], "在线");
        strcpy(online, on[c]);
        message_ui(friendname, online);
    }
}

// 子线程只接收聊天业务
void *recv_work(void *arg) {
    int fd = *(int *)arg;
    char buf[512];
    int recv_ret = -1;
    struct msg cmsg;
    while (1) {
        bzero(buf, sizeof(buf));
        int recv_ret = recv(fd, buf, sizeof(buf), 0);
        if (recv_ret == 0) {
            close(fd);
            pthread_exit(NULL);
        }
        if (recv_ret > 0) {
            cmsg = parse_msg(buf);
            switch (cmsg.code) {
            // 收到添加好友的请求
            case ADD_FRIEND_REQUEST: {
                char c;
                printf("您收到一条好友申请验证\n");
                printf("请输入是否同意添加好友(y/n)?\n");
                scanf("%s", &c);
                if (c == 'y') {
                    printf("已同意\n");
                    send_msg(serv_sock, ADD_FRIEND_ADMIT, "已同意添加好友");
                    fun_main();
                } else {
                    printf("已拒绝\n");
                    send_msg(serv_sock, ADD_FRIEND_REFUSE, "已拒绝添加好友");
                    fun_main();
                }
            }
            }
        }
    }
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

    // 子线程只接收聊天内容
    pthread_t tid;
    pthread_create(&tid, NULL, recv_work, (void *)&serv_sock);
    pthread_detach(tid);

    // 主线程同时接收及发送
    char buf[512];
    int ret = -1;
    struct msg cmsg;
    // 登录界面及操作
    fun_login();
    // 接收服务器返回
    while (1) {
        ret = recv(serv_sock, buf, sizeof(buf), 0);
        if (ret > 0) {
            cmsg = parse_msg(buf);
            switch (cmsg.code) {
            // 注册成功
            case SIGN_UP_SUCCESS: {
                printf("%s\n", cmsg.data);
                fun_login();
                break;
            }
            // 注册用户名已存在
            case SIGN_UP_DUP_USER: {
                printf("%s\n", cmsg.data);
                printf("请重新注册\n");
                fun_login();
                break;
            }
            // 登录成功
            case SIGN_IN_SUCCESS: {
                printf("%s\n", cmsg.data);
                fun_main();
                break;
            }
            // 登录密码错误
            case SIGN_IN_PW_ERROR: {
                printf("%s\n", cmsg.data);
                printf("请重新登录\n");
                fun_login();
                break;
            }
            // 登录用户名不存在
            case SIGN_IN_NO_USER: {
                printf("%s\n", cmsg.data);
                printf("请重新登录\n");
                fun_login();
                break;
            }
            // 退出登录成功
            case SIGN_OUT_SUCCESS: {
                printf("%s\n", cmsg.data);
                fun_login();
                break;
            }
            // 注销用户成功
            case SIGN_DEL_SUCCESS: {
                printf("%s\n", cmsg.data);
                fun_login();
                break;
            }
            // 注销用户不存在
            case SIGN_DEL_NO_USER: {
                printf("%s\n", cmsg.data);
                fun_main();
                break;
            }
            // 注销用户密码错误
            case SIGN_DEL_PW_ERROR: {
                printf("%s\n", cmsg.data);
                fun_main();
                break;
            }
            // 还没有好友
            case NO_FRIENDS: {
                printf("%s\n", cmsg.data);
                break;
            }
            // 接收一个个好友加入数组
            case LISTING_FRIEND: {
                int i;
                struct friend_data fdata;
                bzero(&fdata, sizeof(fdata));
                fdata = parse_fdata(cmsg.data);
                for (i = 0; i < 100; i++) {
                    data[i] = fdata.friendname;
                    if (fdata.is_online) {
                        on[i] = "在线";
                    } else {
                        on[i] = "离线";
                    }
                    count++;
                }
                break;
            }
            // 拉取好友列表
            case LISTING_FRIEND_FINISHED: {
                friend_main();
                break;
            }
            // 收到添加好友的请求
            case ADD_FRIEND_REQUEST: {
                char c;
                printf("您收到一条好友申请验证\n");
                printf("请输入是否同意添加好友(y/n)?\n");
                scanf("%s", &c);
                if (c == 'y') {
                    printf("已同意\n");
                    send_msg(serv_sock, ADD_FRIEND_ADMIT, "已同意添加好友");
                    fun_main();
                } else {
                    printf("已拒绝\n");
                    send_msg(serv_sock, ADD_FRIEND_REFUSE, "已拒绝添加好友");
                    fun_main();
                }
                break;
            }
            // 没有找到该用户
            case ADD_FRIEND_NO_USER: {
                fun_main();
                break;
            }
            // 已经是好友
            case ADD_FRIEND_DUP: {
                fun_main();
                break;
            }
            // 已经被对方拉黑或已拉黑对方
            case ADD_FRIEND_BLACK: {
                fun_main();
                break;
            }
            default:
                break;
            }
        }
    }
    close(serv_sock);
    return 0;
}
