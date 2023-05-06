#ifndef __JSON_DATA_H__
#define __JSON_DATA_H__

struct data {
  int code;
  char msg[256];
  char data[1024];
};

extern struct data parse_json(char *json_str);
extern char *pack_json(struct data data);

#endif