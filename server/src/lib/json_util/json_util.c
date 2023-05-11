#include "json_util.h"

struct msg parse_msg(const char *json_str) {
    struct msg smsg;
    memset(&smsg, 0, sizeof(smsg));

    struct json_object *object = json_tokener_parse(json_str);

    struct json_object *obj_code_int = json_object_object_get(object, "code");
    struct json_object *obj_data_str = json_object_object_get(object, "data");

    smsg.code = json_object_get_int(obj_code_int);
    strcpy(smsg.data, json_object_get_string(obj_data_str));

    return smsg;
}

void pack_msg(struct msg smsg, char *json_str) {
    struct json_object *obj_code = json_object_new_int(smsg.code);
    struct json_object *obj_data = json_object_new_string(smsg.data);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object, "code", obj_code);
    json_object_object_add(object, "data", obj_data);

    strcpy(json_str, json_object_to_json_string(object));
}

struct sign_data parse_signdata(const char *json_str) {
    struct sign_data ssign_data;
    memset(&ssign_data, 0, sizeof(ssign_data));

    struct json_object *object = json_tokener_parse(json_str);

    struct json_object *obj_username_str = json_object_object_get(object, "username");
    struct json_object *obj_password_str = json_object_object_get(object, "password");

    strcpy(ssign_data.username, json_object_get_string(obj_username_str));
    strcpy(ssign_data.password, json_object_get_string(obj_password_str));

    return ssign_data;
}

void pack_signdata(struct sign_data ssign_data, char *json_str) {
    struct json_object *obj_username = json_object_new_string(ssign_data.username);
    struct json_object *obj_password = json_object_new_string(ssign_data.password);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object, "username", obj_username);
    json_object_object_add(object, "password", obj_password);

    strcpy(json_str, json_object_to_json_string(object));
}

struct add_friend_data parse_afdata(const char *json_str) {
    struct add_friend_data saddf_data;
    memset(&saddf_data, 0, sizeof(saddf_data));

    struct json_object *object = json_tokener_parse(json_str);

    struct json_object *obj_username_str = json_object_object_get(object, "username");
    struct json_object *obj_friendname_str = json_object_object_get(object, "friendname");

    strcpy(saddf_data.username, json_object_get_string(obj_username_str));
    strcpy(saddf_data.friendname, json_object_get_string(obj_friendname_str));

    return saddf_data;
}

void pack_afdata(struct add_friend_data saddf_data, char *json_str) {
    struct json_object *obj_username = json_object_new_string(saddf_data.username);
    struct json_object *obj_friendname = json_object_new_string(saddf_data.friendname);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object, "username", obj_username);
    json_object_object_add(object, "friendname", obj_friendname);

    strcpy(json_str, json_object_to_json_string(object));
}

struct fmsg_data parse_fmdata(const char *json_str) {
    struct fmsg_data fm_data;
    memset(&fm_data, 0, sizeof(fm_data));

    struct json_object *object = json_tokener_parse(json_str);

    struct json_object *obj_uuid_str = json_object_object_get(object, "uuid");
    struct json_object *obj_msg_str = json_object_object_get(object, "msg");
    struct json_object *obj_sendname_str = json_object_object_get(object, "send_name");
    struct json_object *obj_recvname_str = json_object_object_get(object, "recv_name");
    struct json_object *obj_sendat_str = json_object_object_get(object, "send_at");
    struct json_object *obj_recvat_str = json_object_object_get(object, "recv_at");

    strcpy(fm_data.msg, json_object_get_string(obj_msg_str));
    strcpy(fm_data.uuid, json_object_get_string(obj_uuid_str));
    strcpy(fm_data.send_name, json_object_get_string(obj_sendname_str));
    strcpy(fm_data.recv_name, json_object_get_string(obj_recvname_str));
    strcpy(fm_data.recv_at, json_object_get_string(obj_recvat_str));
    strcpy(fm_data.recv_at, json_object_get_string(obj_recvat_str));

    return fm_data;
}
void pack_fmdata(struct fmsg_data fmdata, char *json_str) {
    struct json_object *obj_msg = json_object_new_string(fmdata.msg);
    struct json_object *obj_sendname = json_object_new_string(fmdata.send_name);
    struct json_object *obj_recvname = json_object_new_string(fmdata.recv_name);
    struct json_object *obj_sendat = json_object_new_string(fmdata.send_at);
    struct json_object *obj_recvat = json_object_new_string(fmdata.recv_at);
    struct json_object *obj_uuid = json_object_new_string(fmdata.uuid);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object, "uuid", obj_uuid);
    json_object_object_add(object, "msg", obj_msg);
    json_object_object_add(object, "user_name", obj_sendname);
    json_object_object_add(object, "send_name", obj_recvname);
    json_object_object_add(object, "recv_at", obj_sendat);
    json_object_object_add(object, "send_at", obj_recvat);

    strcpy(json_str, json_object_to_json_string(object));
}

struct friend_data parse_fdata(const char *json_str) {
    struct friend_data fdata;
    memset(&fdata, 0, sizeof(fdata));

    struct json_object *object = json_tokener_parse(json_str);

    struct json_object *obj_friendname_str = json_object_object_get(object, "friendname");
    struct json_object *obj_isonline_char = json_object_object_get(object, "is_online");

    strcpy(fdata.friendname, json_object_get_string(obj_friendname_str));
    fdata.is_online = json_object_get_int(obj_isonline_char);

    return fdata;
}

void pack_fdata(struct friend_data fdata, char *json_str) {
    struct json_object *obj_friendname = json_object_new_string(fdata.friendname);
    struct json_object *obj_isonline = json_object_new_string(fdata.is_online);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object, "friendname", obj_friendname);
    json_object_object_add(object, "is_online", obj_isonline);

    strcpy(json_str, json_object_to_json_string(object));
}