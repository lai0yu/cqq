#ifndef __JSON_UTIL_H__
#define __JSON_UTIL_H__

#include <json/json.h>
#include <stdio.h>
#include <string.h>

struct msg
{
	int code;
	char cmd[256];
	char data[1024];
};

extern struct msg parse_msg(char* json_str);
extern const char* pack_msg(struct msg pack);

#endif