#include "json_data.h"

struct data parse_json(char *json_str) {
  struct data pack_data;

  struct json_object *object = json_tokener_parse(json_str);

  // 将各个数据根据key从大对象中提取出来
  struct json_object *obj_codes = json_object_object_get(object, "code");
  struct json_object *obj_msgs = json_object_object_get(object, "msg");
  struct json_object *obj_datas = json_object_object_get(object, "data");

  // 将各个json的小对象数据转化为结构体数据
  pack_data.code = json_object_get_int(obj_codes);
  strcpy(pack_data.msg, json_object_get_string(obj_msgs));
  strcpy(pack_data.data, json_object_get_string(obj_datas));

  return pack_data;
}

char *pack_json(struct data pack_data) {

  struct json_object *obj_code = json_object_new_int(pack_data.code); // 将结构体中的数据转化为json小对象
  struct json_object *obj_msg = json_object_new_string(pack_data.msg);
  struct json_object *obj_data = json_object_new_string(pack_data.data);

  // 创建一个json大对象
  struct json_object *object = json_object_new_object();

  // 将json小对象放入大对象中,并且打标签
  json_object_object_add(object, "code", obj_code);
  json_object_object_add(object, "msg", obj_msg);
  json_object_object_add(object, "data", obj_data);

  // 将json大对象转化为字符串
  const char *p = json_object_to_json_string(object);

  return p;
}
