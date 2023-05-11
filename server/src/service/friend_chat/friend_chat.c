#include "friend_chat.h"

static int send_msg(int socket, char code, const char *data) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(struct msg));
    smsg.code = code;
    strcpy(smsg.data, data);

    char msg_buf[2048] = {0};
    pack_msg(smsg, msg_buf);
    return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

int init_friend_chat_service() {
    return exec_sql(sql_create_fmsg_table, NULL, NULL);
}

int send_msg_to_friend(const char *data, int socket) {
    struct fmsg_data fmd = parse_fmdata(data);

    char wherebuf[64] = {0};
    sprintf(wherebuf, "username=='%s'", fmd.recv_name);
    struct select_row *f_select_head = db_select("tb_account", "socket", wherebuf);

    int f_select_count = list_entry(f_select_head->list.prev, struct select_row, list)->index;

    if (f_select_count < 1) {
        send_msg(socket, SEND_MSG_NO_USER, "未知的好友用户名");
    } else {
        int f_socket = atoi(list_entry(f_select_head->list.prev, struct select_row, list)->argv[0]);

        if (f_socket > -1) {
            send_msg(f_socket, SEND_MSG_TO_FRIEND, data);
        }
        db_insert("tb_fmsg(send_name, recv_name,code,data)", "");
    }
}

int list_offline_msg(const char *data, int socket) {
    return 0;
}
