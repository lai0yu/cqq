#include "json_util.h"

struct msg parse_msg(char* json_str)
{
	struct msg pack;

	struct json_object* object = json_tokener_parse(json_str);

	struct json_object* obj_code_str = json_object_object_get(object, "code");
	struct json_object* obj_msg_str = json_object_object_get(object, "cmd");
	struct json_object* obj_data_str = json_object_object_get(object, "data");

	pack.code = json_object_get_int(obj_code_str);
	strcpy(pack.cmd, json_object_get_string(obj_msg_str));
	strcpy(pack.data, json_object_get_string(obj_data_str));

	return pack;
}

const char* pack_msg(struct msg pack)
{

	struct json_object* obj_code = json_object_new_int(pack.code);
	struct json_object* obj_msg = json_object_new_string(pack.cmd);
	struct json_object* obj_data = json_object_new_string(pack.data);

	struct json_object* object = json_object_new_object();

	json_object_object_add(object, "code", obj_code);
	json_object_object_add(object, "cmd", obj_msg);
	json_object_object_add(object, "data", obj_data);

	const char* p = json_object_to_json_string(object);

	return p;
}
