#ifndef __JSON_DATA_H__
#define __JSON_DATA_H__

#include <json/json.h>
#include <string.h>

struct msg_data
{
	int code;
	char msg[256];
	char data[1024];
};

extern struct msg_data parse_json(char* json_str);
extern char* pack_json(struct msg_data data);

#endif