#include "friend.h"

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

int init_friend_service() {
    return exec_sql(sql_create_friend_table, NULL, NULL);
}

int list_friends(const char *data, int socket) {
    char where_buf[256] = {0};
    sprintf(where_buf, "username=='%s' or friendname=='%s'", data, data);
    struct select_row *frow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf);

    int count = list_entry(frow_head->list.prev, struct select_row, list)->index;

    if (count < 1) {
        send_msg(socket, NO_FRIENDS, "您还没有添加过好友");
    } else {
        struct select_row *pos;
        const char *friend_name;
        char *is_online_str;
        char send_data[256] = {0};
        int relation = -1;
        list_for_each_entry(pos, &frow_head->list, list) {
            relation = atoi(pos->argv[2]);
            if (relation != 2 && relation != 4) {
                continue;
            }
            friend_name = strcmp(data, pos->argv[0]) == 0 ? pos->argv[0] : data;
            bzero(where_buf, sizeof(where_buf));
            sprintf(where_buf, "username=='%s'", friend_name);
            struct select_row *socket_status =
                db_select("tb_account", "socket", where_buf);
            char is_online =
                atoi(list_entry(socket_status->list.next, struct select_row, list)->argv[0]) > -1 ? 1 : 0;

            struct friend_data fdata;

            strcpy(fdata.friendname, friend_name);
            fdata.is_online = is_online;
            fdata.unrecv_msg_count = 5;

            char friend_data_str[512] = {0};
            pack_fdata(fdata, friend_data_str);

            send_msg(socket, LISTING_FRIEND, friend_data_str);
            free_select_rows_list(socket_status);
        }
        send_msg(socket, LISTING_FRIEND_FINISHED, "所有好友都已经发送");
    }
    free_select_rows_list(frow_head);
    return 0;
}

int add_friend(const char *data, int socket) {
    struct add_friend_data afdata = parse_afdata(data);

    char where_buf1[256] = {0};
    sprintf(where_buf1, "username=='%s' and friendname=='%s'", afdata.username,
            afdata.friendname);
    struct select_row *u2ffrow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf1);

    int u2fcount =
        list_entry(u2ffrow_head->list.prev, struct select_row, list)->index;

    char where_buf2[256] = {0};
    sprintf(where_buf2, "username=='%s' and friendname=='%s'", afdata.friendname,
            afdata.username);
    struct select_row *f2urow_head =
        db_select("tb_friend", "username,friendname,relation", where_buf2);
    int f2ucount =
        list_entry(f2urow_head->list.prev, struct select_row, list)->index;

    struct select_row *row_head = u2fcount > 0 ? u2ffrow_head : f2urow_head;
    int is_reverse = u2fcount > 0 ? 0 : 1;
    int relation = atoi(list_entry(row_head->list.next, struct select_row, list)->argv[2]);
    char *oppsite_name = is_reverse ? afdata.friendname : afdata.username;

    char where_buf[256] = {0};
    bzero(where_buf, sizeof(where_buf));
    sprintf(where_buf, "username=='%s'", oppsite_name);
    struct select_row *sock_head = db_select("tb_account", "socket", where_buf);

    int sock_count = list_entry(sock_head->list.prev, struct select_row, list)->index;
    if (sock_count < 1) {
        send_msg(socket, ADD_FRIEND_NO_USER, "无此用户");
        return -1;
    }

    int oppsite_socket = atoi(list_entry(sock_head->list.prev, struct select_row, list)->argv[0]);

    if (f2ucount == 0 && u2fcount == 0) {
        char values_buf[256] = {0};
        sprintf(values_buf, "'%s','%s',1", afdata.username, afdata.friendname);
        db_insert("tb_friend(username,friendname,relation)", values_buf);
        if (oppsite_socket > 0) {
            send_msg(oppsite_socket, ADD_FRIEND_REQUEST, data);
        }
    } else {
        char *relation_set;
        switch (relation) {
        case 0:
            if (oppsite_socket > 0) {
                send_msg(oppsite_socket, ADD_FRIEND_REQUEST, data);
            }
            char values_buf0[256] = {0};
            uuid_t uuid0;
            uuid_generate(uuid0);
            sprintf(
                values_buf0,
                "'%s','%s','%s',%d,'%s'",
                uuid0, afdata.username, afdata.friendname, ADD_FRIEND_REQUEST, data);
            db_insert("tb_fmsg(uuid,send_name,recv_name,code,data)", values_buf0);
            relation_set = !is_reverse ? "relation=1" : "relation=3";
            db_update("tb_friend", relation_set, !is_reverse ? where_buf1 : where_buf2);
            break;
        case 1:
            if (oppsite_socket > 0) {
                send_msg(oppsite_socket, ADD_FRIEND_REQUEST, data);
            }
            char values_buf1[256] = {0};
            uuid_t uuid1;
            uuid_generate(uuid1);
            sprintf(
                values_buf1,
                "'%s','%s','%s',%d,'%s'",
                uuid1, afdata.username, afdata.friendname, ADD_FRIEND_REQUEST, data);
            db_insert("tb_fmsg(uuid,send_user,recv_user,code,data)", values_buf1);
            relation_set = !is_reverse ? "relation=1" : "relation=3";
            db_update("tb_friend", relation_set, !is_reverse ? where_buf1 : where_buf2);
            break;
        case 2:
            send_msg(socket, ADD_FRIEND_DUP, data);
            break;
        case 3:
            if (oppsite_socket > 0) {
                send_msg(oppsite_socket, ADD_FRIEND_REQUEST, data);
            }
            char values_buf3[256] = {0};
            uuid_t uuid3;
            uuid_generate(uuid3);
            sprintf(
                values_buf3,
                "'%s','%s','%s',%d,'%s'",
                uuid3, afdata.username, afdata.friendname, ADD_FRIEND_REQUEST, data);

            db_insert("tb_fmsg(uuid,send_name,recv_name,code,data)", values_buf3);
            relation_set = !is_reverse ? "relation=1" : "relation=3";
            db_update("tb_friend", relation_set, !is_reverse ? where_buf1 : where_buf2);
            break;
        case 4:
            send_msg(socket, ADD_FRIEND_DUP, data);
            break;
        case 5:
            send_msg(socket, ADD_FRIEND_BLACK, data);
            break;
        case 7:
            send_msg(socket, ADD_FRIEND_BLACK, data);
            break;
        default:
            break;
        }
    }

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

        if (!is_reverse) {
            sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                    afdata.friendname);
            sprintf(set_buf, "relation=%d", 2);

        } else {
            sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                    afdata.username);
            sprintf(set_buf, "relation=%d", 4);
        }

        db_update("tb_friend", set_buf, where_buf);

        bzero(where_buf, sizeof(where_buf));
        sprintf(where_buf, "recv_name=='%s' and code==%d and recv_time==null", afdata.username, ADD_FRIEND_REQUEST);
        struct select_row *offline_friend_add_head = db_select("tb_fmsg", "uuid", where_buf);
        int offline_friend_add_count = list_entry(offline_friend_add_head->list.prev, struct select_row, list)->index;

        if (offline_friend_add_count > 0) {
            struct select_row *pos = NULL;
            list_for_each_entry(pos, &offline_friend_add_head->list, list) {
                bzero(where_buf, sizeof(where_buf));
                sprintf(where_buf, "uuid=='%s'", pos->argv[0]);
                db_update("tb_fmsg", "recv_at=(datetime('now','localtime'))", where_buf);
            }
        }
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

        if (!is_reverse) {
            sprintf(where_buf, "username=='%s' and friendname=='%s'", afdata.username,
                    afdata.friendname);
            sprintf(set_buf, "relation=%d", 0);

        } else {
            sprintf(set_buf, "username=='%s' and friendname=='%s'", afdata.friendname,
                    afdata.username);
            sprintf(set_buf, "relation=%d", 0);
        }

        db_update("tb_friend", set_buf, where_buf);

        bzero(where_buf, sizeof(where_buf));
        sprintf(where_buf, "recv_name=='%s' and code==%d and recv_time==null", afdata.username, ADD_FRIEND_REQUEST);
        struct select_row *offline_friend_add_head = db_select("tb_fmsg", "uuid", where_buf);
        int offline_friend_add_count = list_entry(offline_friend_add_head->list.prev, struct select_row, list)->index;

        if (offline_friend_add_count > 0) {
            struct select_row *pos = NULL;
            list_for_each_entry(pos, &offline_friend_add_head->list, list) {
                bzero(where_buf, sizeof(where_buf));
                sprintf(where_buf, "uuid=='%s'", pos->argv[0]);
                db_update("tb_fmsg", "recv_at=(datetime('now','localtime'))", where_buf);
            }
        }
    }

    return 0;
}

int del_friend(const char *data, int socket) {
}