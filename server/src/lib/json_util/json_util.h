#ifndef __JSON_UTIL_H__
#define __JSON_UTIL_H__

#include <json/json.h>
#include <stdio.h>
#include <string.h>

//
struct msg {
	signed char code;
	char data[1023];
};

extern struct msg parse_msg(const char* json_str);
extern void pack_msg(struct msg smsg, char* json_str);

struct sign_data {
	char username[64];
	char password[64];
};

extern struct sign_data parse_signdata(const char* json_str);
extern void pack_signdata(struct sign_data ssign_data, char* json_str);
#endif