#ifndef __JSON_UTIL_H__
#define __JSON_UTIL_H__

#include <json/json.h>
#include <stdio.h>
#include <string.h>

//
struct msg {
    char code;
    char data[1023];
};

extern struct msg parse_msg(const char *json_str);
extern void pack_msg(struct msg smsg, char *json_str);

struct sign_data {
    char username[64];
    char password[64];
};

extern struct sign_data parse_signdata(const char *json_str);
extern void pack_signdata(struct sign_data ssign_data, char *json_str);

struct add_friend_data {
    char username[64];
    char friendname[64];
};

extern struct add_friend_data parse_afdata(const char *json_str);
extern void pack_afdata(struct add_friend_data ssign_data, char *json_str);

struct fmsg_data {
    char uuid[64];
    char send_name[64];
    char recv_name[64];
    unsigned int send_at;
    unsigned int recv_at;
    char msg[1023 - 64 - 64 - 64 - 64];
};

extern struct fmsg_data parse_fmdata(const char *json_str);
extern void pack_fmdata(struct fmsg_data fmdata, char *json_str);

struct friend_data {
    char friendname[64];
    char is_online;
};

extern struct friend_data parse_fdata(const char *json_str);
extern void pack_fdata(struct friend_data fdata, char *json_str);

#endif