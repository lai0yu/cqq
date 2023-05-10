#include "json_util.h"

struct msg parse_msg(const char* json_str)
{
	struct msg smsg;
	memset(&smsg, 0, sizeof(smsg));

	struct json_object* object = json_tokener_parse(json_str);

	struct json_object* obj_code_int = json_object_object_get(object, "code");
	struct json_object* obj_data_str = json_object_object_get(object, "data");

	smsg.code = json_object_get_int(obj_code_int);
	strcpy(smsg.data, json_object_get_string(obj_data_str));

	return smsg;
}

void pack_msg(struct msg smsg, char* json_str)
{
	struct json_object* obj_code = json_object_new_int(smsg.code);
	struct json_object* obj_data = json_object_new_string(smsg.data);

	struct json_object* object = json_object_new_object();

	json_object_object_add(object, "code", obj_code);
	json_object_object_add(object, "data", obj_data);

	strcpy(json_str, json_object_to_json_string(object));
}

struct sign_data parse_signdata(const char* json_str)
{
	struct sign_data ssign_data;
	memset(&ssign_data, 0, sizeof(ssign_data));

	struct json_object* object = json_tokener_parse(json_str);

	struct json_object* obj_username_str = json_object_object_get(object, "username");
	struct json_object* obj_password_str = json_object_object_get(object, "password");

	strcpy(ssign_data.username, json_object_get_string(obj_username_str));
	strcpy(ssign_data.password, json_object_get_string(obj_password_str));

	return ssign_data;
}

void pack_signdata(struct sign_data ssign_data, char* json_str)
{
	struct json_object* obj_username = json_object_new_string(ssign_data.username);
	struct json_object* obj_password = json_object_new_string(ssign_data.password);

	struct json_object* object = json_object_new_object();

	json_object_object_add(object, "username", obj_username);
	json_object_object_add(object, "password", obj_password);

	strcpy(json_str, json_object_to_json_string(object));
}
