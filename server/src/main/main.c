#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../service/account/account.h"
#include "../service/friend/friend.h"
#include "../service/friend_chat/friend_chat.h"

// 最大连接数
#define MAX_CONNECT 800

struct client {
    int socket;
    struct sockaddr_in clientaddr;
    unsigned int clientaddr_size;
    char username[64];
};

int clients_count = 0;
int capcity = 5;

struct client *clients;

void init_clients() {
    clients = (struct client *)malloc(sizeof(struct client) * 5);
}

void add_client(int socket, struct sockaddr_in client_addr, char *username) {
    if (clients_count == capcity) {
        capcity = 2 * capcity;
        clients = (struct client *)realloc(clients, sizeof(struct client) * capcity);
    }
    clients[clients_count].socket = socket;
    clients[clients_count].clientaddr = client_addr;
    strcpy(clients[clients_count].username, username);
    clients_count++;
}

void del_client_by_socket(int socket) {
    int i, j;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].socket == socket) {
            for (j = i; j < clients_count - 1; j++) {
                clients[j] = clients[j + 1];
                update_socket(socket, -1);
            }
            clients_count--;
            break;
        }
    }
}

void set_client_username(int socket, char *username) {
    int i;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].socket == socket) {
            strcpy(clients[i].username, username);
            break;
        }
    }
}

void set_client_sockaddr(int socket, struct sockaddr_in clientaddr) {
    int i;
    for (i = 0; i < clients_count; i++) {
        if (clients[i].socket == socket) {
            clients[i].clientaddr = clientaddr;
            break;
        }
    }
}

void free_clients() {
    int i;
    for (i = 0; i < clients_count; i++) {
        del_client_by_socket(clients[i].socket);
    }
    free(clients);
}

void serve(int csocket, int code, const char *data) {
    switch (code) {
    case SIGN_UP: sign_up(data, csocket); break;
    case SIGN_DEL: sign_del(data, csocket); break;
    case SIGN_IN: sign_in(data, csocket); break;
    case SIGN_OUT: sign_out(data, csocket); break;
    case PULL_FRIENDS_LIST: list_friends(data, csocket); break;
    case ADD_FRIEND: add_friend(data, csocket); break;
    case ADD_FRIEND_ADMIT: add_friend_admit(data, csocket); break;
    case ADD_FRIEND_REFUSE: add_friend_refuse(data, csocket); break;
    default:
        break;
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

    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
    serv_addr.sin_port = htons(atoi(serv_port));
    bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, MAX_CONNECT);

    init_clients();

    db_open();
    init_account_service();
    init_friend_service();
    init_friend_chat_service();

    fd_set fdset;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    char recv_buf[2048] = {0};
    while (1) {
        FD_ZERO(&fdset);
        FD_SET(serv_sock, &fdset);

        int k;
        for (k = 0; k < clients_count; k++) {
            if (clients[k].socket > 0) { FD_SET(clients[k].socket, &fdset); }
        }

        int select_ret = select(MAX_CONNECT + 1, &fdset, NULL, NULL, &tv);
        if (select_ret < 0) {
            perror("select failed");
            break;
        } else if (select_ret == 0) {
            continue;
        } else {
            if (FD_ISSET(serv_sock, &fdset)) {
                struct sockaddr_in caddr;
                unsigned int caddr_size;
                int csocket = accept(serv_sock, (struct sockaddr *)&caddr, &caddr_size);
                if (csocket > 0) {
                    if (clients_count < MAX_CONNECT) {
                        add_client(csocket, caddr, "");
                        printf("A new client %s:%d has connected!\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
                    } else {
                        printf("Refuse %s:%d connect cause MAX connect!\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
                        close(csocket);
                    }
                }
            }
            int i;
            for (i = 0; i < clients_count; i++) {
                if (FD_ISSET(clients[i].socket, &fdset)) {
                    bzero(recv_buf, sizeof(recv_buf));
                    int recv_ret = recv(clients[i].socket, recv_buf, sizeof(recv_buf), 0);
                    if (recv_ret > 0) {
                        struct msg cmsg = parse_msg(recv_buf);
                        serve(clients[i].socket, cmsg.code, cmsg.data);
                        printf("msg from client%s:%d==>%s\n",
                               inet_ntoa(clients[i].clientaddr.sin_addr),
                               ntohs(clients[i].clientaddr.sin_port), recv_buf);
                    } else if (recv_ret == 0) {
                        printf("socket:%d has send zero length buf\n", clients[i].socket);

                        int j;
                        for (j = 0; j < clients_count; j++) {
                            if (clients[i].socket == clients[i].socket) {
                                printf("clients: %s %d will be close cause of wrong length msg\n",
                                       inet_ntoa(clients[i].clientaddr.sin_addr),
                                       ntohs(clients[i].clientaddr.sin_port));

                                break;
                            }
                        }

                        if (select_ret == 1) {
                            del_client_by_socket(clients[i].socket);
                        }
                    }
                }
            }
        }
    }

    close(serv_sock);
    free_clients();
    return 0;
}
