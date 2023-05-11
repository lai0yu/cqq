#include "friend_chat.h"

static int send_msg(int socket, char code, const char *data) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(struct msg));
    smsg.code = code;
    strcpy(smsg.data, data);

    char msg_buf[2048] = {0};
    pack_msg(smsg, msg_buf);
    printf("%s\n", msg_buf);
    return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

int init_friend_chat_service() {
    return exec_sql(sql_create_fmsg_table, NULL, NULL);
}

int send_msg_to_friend(const char *data, int socket) {
    struct fmsg_data fmd = parse_fmdata(data);

    char values_buf[4096] = {0};
    sprintf(
        values_buf, "('%s','%s','%s','%s',%d,'%s',datetime(%ud, 'unixepoch', 'localtime'))",
        fmd.uuid, fmd.send_name, fmd.recv_name, fmd.msg, SEND_MSG_TO_FRIEND, data, fmd.send_at);

    db_insert("tb_fmsg(uuid,send_name,recv_name,msg,code,data,send_at)", values_buf);

    char wherebuf[256] = {0};
    struct select_row *f_select_head = db_select("tb_account", "socket", wherebuf);

    int f_select_count = list_entry(f_select_head->list.prev, struct select_row, list)->index;
    if (f_select_count < 1) {
        send_msg(socket, SEND_MSG_NO_USER, "未知的好友用户名");
    } else {
        int f_socket = atoi(list_entry(f_select_head->list.prev, struct select_row, list)->argv[0]);

        if (f_socket > -1) {
            send_msg(f_socket, SEND_MSG_TO_FRIEND_REQUEST, data);
        }
    }
    return 0;
}

int recv_friend_msg(const char *data, int socket) {
    struct fmsg_data fmd = parse_fmdata(data);

    char sets_buf[256] = {0};
    sprintf(sets_buf, "recv_at='datetime(%ud,'unixepoch', 'localtime')'", fmd.recv_at);

    char where_buf[256] = {0};
    sprintf(where_buf, "uuid=='%s'", fmd.uuid);
    db_update("tb_fmsg", sets_buf, where_buf);
    return 0;
}

int list_offline_msg(const char *data, int socket) {
    char where_buf[256] = {0};
    sprintf(where_buf, "recv_name=='%s' and recv==null", data);
    struct select_row *msg_list_head = db_select("tb_fmsg", "uuid,send_name,recv_name,msg,strftime(send_at),strftime(recv_at)", where_buf);

    int msg_count = list_entry(msg_list_head->list.prev, struct select_row, list)->index;

    if (msg_count > 0) {
        struct select_row *pos = NULL;
        struct fmsg_data fmd;
        char msg_buf[2048] = {0};
        list_for_each_entry(pos, &msg_list_head->list, list) {
            char msg_buf[1024];
            memset(&fmd, 0, sizeof(struct fmsg_data));
            strcpy(fmd.uuid, pos->argv[0]);
            strcpy(fmd.send_name, pos->argv[1]);
            strcpy(fmd.recv_name, pos->argv[2]);
            strcpy(fmd.msg, pos->argv[3]);
            fmd.send_at = atoi(pos->argv[4]);
            fmd.recv_at = atoi(pos->argv[5]);
            bzero(msg_buf, sizeof(msg_buf));
            pack_fmdata(fmd, msg_buf);
            send_msg(socket, LISTING_OFFLINE_MSG, msg_buf);
        }
    }

    send_msg(socket, LISTING_OFFLINE_FINISH, "");

    return 0;
}
