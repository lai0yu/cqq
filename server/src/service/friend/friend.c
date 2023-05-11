#include "friend.h"

static int send_msg(int socket, char code, char *data) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(struct msg));
    smsg.code = code;
    strcpy(smsg.data, data);

    char msg_buf[1024] = {0};
    pack_msg(smsg, msg_buf);
    return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

int init_single_chat_service() {
    return exec_sql(sql_create_friend_table, NULL, NULL);
}

int list_friends(const char *data, int socket) {
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' or friendname=='%s'", data, data);
    struct select_row *frow_head =
        db_select("tb friend", "username,friendname,relation", where_buf);

    int count = list_entry(frow_head->list.prev, struct select_row, list)->index;

    if (count < 1) {
        send_msg(socket, NO_FRIENDS, "您还没有添加过好友");
    } else {
        struct select_row *pos;
        const char *friend_name;
        char *is_online_str;
        char send_data[256] = {0};
        list_for_each_entry(pos, &frow_head->list, list) {
            if (atoi(pos->argv[2]) != 2 && atoi(pos->argv[2] != 4)) {
                continue;
            }
            friend_name = strcmp(data, pos->argv[0]) != 0 ? data : pos->argv[0];
            bzero(where_buf, sizeof(where_buf));
            sprintf(where_buf, "username=='%s'", friend_name);
            struct select_row *socket_status =
                db_select("tb_account", "socket", where_buf);
            is_online_str =
                list_entry(socket_status->list.next, struct select_row, list)
                            ->argv[0]
                        > 0 ?
                    "在线" :
                    "离线";
            sprintf(send_data, "%s(%s)", friend_name, is_online_str);
            send_msg(socket, LISTING_FRIEND, send_data);
            free_select_rows_list(socket_status);
        }
        send_msg(socket, LISTING_FRIEND_FINISHED, "所有好友都已经发送");
    }
    free_select_rows_list(frow_head);
    return 0;
}

int add_friend(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);

    char where_buf[256] = {0};
    sprintf(where_buf,
            "(username=='%s' and friendname=='%s') or (friendname=='%s' and "
            "username=='%s')",
            afdata.username, afdata.friendname, afdata.username,
            afdata.friendname);
    struct select_row *frow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);

    int count = list_entry(frow_head->list.prev, struct select_row, list)->index;

    if (count < 0) {
        bzero(where_buf, sizeof(where_buf));
        sprintf(where_buf, "username=='%s'", afdata.friendname);
        struct select_row *sign_state =
            db_select("tb_account", "socket", where_buf);

        int socket = atoi(sign_state->argv[0]);

        if (socket > 0) {
            send_msg(socket, ADD_FRIEND, afdata.username);
        }
        char values_buf[256] = {0};
        sprintf(values_buf, "('%s','%s',%d)", afdata.username, afdata.friendname,
                1);
        db_insert("tb_friend(username,friendname,relation)", values_buf);
        free_select_rows_list(sign_state);
    } else {
        struct select_row *first_row =
            list_entry(frow_head->list.next, struct select_row, list);
        int relation = atoi(first_row->argv[2]);

        if (relation == 2 || relation == 4) {
            send_msg(socket, ADD_FRIEND_DUP, "已经是好友，无需申请");
        } else if (relation == 5 || relation == 7) {
            send_msg(socket, ADD_FRIEND_BLACK, "您已经拉黑对方或被对方拉黑,无法添加");
        } else if (relation == 1) {
            if (strcmp(afdata.username, first_row->argv[0]) == 0) {
            } else {
                char where_buf[256] = {0};
                sprintf(where_buf, "username=='%s' and friendname=='%s'",
                        afdata.friendname, afdata.username);
                db_update("tb_friend", "relation=2", where_buf);
            }
        } else if (relation == 3) {
            if (strcmp(afdata.username, first_row->argv[0]) == 0) {
                char where_buf[256] = {0};
                sprintf(where_buf, "username=='%s' and friendname=='%s'",
                        afdata.username, afdata.friendname);
                db_update("tb_friend", "relation=4", where_buf);
            } else {
            }
        } else if (relation == 0) {
            if (strcmp(afdata.username, first_row->argv[0]) == 0) {
                char where_buf[256] = {0};
                sprintf(where_buf, "username=='%s' and friendname=='%s'",
                        afdata.username, afdata.friendname);
                db_update("tb_friend", "relation=1", where_buf);
            } else {
                char where_buf[256] = {0};
                sprintf(where_buf, "username=='%s' and friendname=='%s'",
                        afdata.friendname, afdata.username);
                db_update("tb_friend", "relation=3", where_buf);
            }

            char where_buf[256] = {0};
            bzero(where_buf, sizeof(where_buf));
            sprintf(where_buf, "username=='%s'", afdata.friendname);
            struct select_row *sign_state =
                db_select("tb_account", "socket", where_buf);

            int socket = atoi(sign_state->argv[0]);

            if (socket > 0) {
                send_msg(socket, ADD_FRIEND, afdata.username);
            }
            free_select_rows_list(sign_state);
        }
    }
    free_select_rows_list(frow_head);
    return 0;
}

int add_friend_admit(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
            afdata.friendname);
    struct select_row *u2ffrow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int u2fcount =
        list_entry(u2ffrow_head->list.prev, struct select_row, list)->index;

    bzero(where_buf, sizeof(where_buf));
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
            afdata.username);
    struct select_row *f2urow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int f2ucount =
        list_entry(f2urow_head->list.prev, struct select_row, list)->index;

    if (f2ucount == 0 && u2fcount == 0) {
        // TODO: no friend relation ship
    } else {
        struct select_row *row_head = u2fcount > 0 ? u2ffrow_head : f2urow_head;

        int is_reverse = u2fcount > 0 ? 0 : 1;

        bzero(where_buf, sizeof(where_buf));
        char set_buf[256] = {0};

        if (is_reverse == 0) {
            sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                    afdata.friendname);
            sprintf(set_buf, "relation=%d", 2);

        } else {
            sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                    afdata.username);
            sprintf(set_buf, "relation=%d", 4);
        }

        db_update("tb_friend", set_buf, where_buf);
    }

    return 0;
}

int add_friend_refuse(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
            afdata.friendname);
    struct select_row *u2ffrow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int u2fcount =
        list_entry(u2ffrow_head->list.prev, struct select_row, list)->index;

    bzero(where_buf, sizeof(where_buf));
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
            afdata.username);
    struct select_row *f2urow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int f2ucount =
        list_entry(f2urow_head->list.prev, struct select_row, list)->index;

    if (f2ucount == 0 && u2fcount == 0) {
        // TODO: no friend relation ship
    } else {
        struct select_row *row_head = u2fcount > 0 ? u2ffrow_head : f2urow_head;

        int is_reverse = u2fcount > 0 ? 0 : 1;

        bzero(where_buf, sizeof(where_buf));
        char set_buf[256] = {0};

        if (is_reverse == 0) {
            sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                    afdata.friendname);
            sprintf(set_buf, "relation=%d", 0);

        } else {
            sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                    afdata.username);
            sprintf(set_buf, "relation=%d", 0);
        }

        db_update("tb_friend", set_buf, where_buf);
    }

    return 0;
}

int del_friend(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
            afdata.friendname);
    struct select_row *u2ffrow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int u2fcount =
        list_entry(u2ffrow_head->list.prev, struct select_row, list)->index;

    bzero(where_buf, sizeof(where_buf));
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
            afdata.username);
    struct select_row *f2urow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int f2ucount =
        list_entry(f2urow_head->list.prev, struct select_row, list)->index;

    if (f2ucount == 0 && u2fcount == 0) {
        // TODO: no friend relation ship
    } else {
        struct select_row *row_head = u2fcount > 0 ? u2ffrow_head : f2urow_head;

        int is_reverse = u2fcount > 0 ? 0 : 1;

        bzero(where_buf, sizeof(where_buf));
        char set_buf[256] = {0};

        if (is_reverse == 0) {
            sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                    afdata.friendname);
            sprintf(set_buf, "relation=%d", 0);

        } else {
            sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                    afdata.username);
            sprintf(set_buf, "relation=%d", 0);
        }
        db_update("tb_friend", set_buf, where_buf);
    }

    return 0;
}

int black_friend(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
            afdata.friendname);
    struct select_row *u2ffrow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int u2fcount =
        list_entry(u2ffrow_head->list.prev, struct select_row, list)->index;

    bzero(where_buf, sizeof(where_buf));
    sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
            afdata.username);
    struct select_row *f2urow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);
    int f2ucount =
        list_entry(f2urow_head->list.prev, struct select_row, list)->index;

    if (f2ucount == 0 && u2fcount == 0) {
        char set_buf[256] = {0};
        sprintf(set_buf, "'%s','%s',5", afdata.username, afdata.friendname);
        db_insert("tb_account(username,friendname,relation)", set_buf);
    }

    struct select_row *row_head = u2fcount > 0 ? u2ffrow_head : f2urow_head;

    int is_reverse = u2fcount > 0 ? 0 : 1;

    bzero(where_buf, sizeof(where_buf));
    char set_buf[256] = {0};

    if (is_reverse == 0) {
        sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                afdata.friendname);
        sprintf(set_buf, "relation=%d", 5);

    } else {
        sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                afdata.username);
        sprintf(set_buf, "relation=%d", 7);
    }

    db_update("tb_friend", set_buf, where_buf);

    return 0;
}
