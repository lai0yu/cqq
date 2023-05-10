#include "single_chat.h"

static int send_msg(int socket, char code, char* data) {
	struct msg smsg;
	memset(&smsg, 0, sizeof(struct msg));
	smsg.code = code;
	strcpy(smsg.data, data);

	char msg_buf[1024] = { 0 };
	pack_msg(smsg, msg_buf);
	return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
}

int init_single_chat_service(){
	return exec_sql(sql_create_friend_table,NULL,NULL);
}

int list_friends(const char* data,int socket){
	char where_buf[256] = {0};
	sprintf(where_buf,"username=='%s' or friendname=='%s'",data,data);
	struct select_row* frow_head = db_select("tb friend","username,friendname,relation",where_buf);
	int count = list_entry(frow_head->list.prev,struct select_row,list)->index;
	if(count < 1){
		send_msg(socket,NO_FRIENDS,"您还没有添加过好友");
	}else{
		struct select_row* pos;
		const char * friend_name;
		char *is_online_str;
		char send_data[256] = {0};
		list_for_each_entry(pos,&frow_head->list,list){
			friend_name = strcmp(data,pos->argv[0]) !=  0 ? data : pos->argv[0];
			bzero(where_buf,sizeof(where_buf));
			sprintf(where_buf,"username=='%s'",friend_name);
			struct select_row* socket_status = db_select("tb_account","socket",where_buf);
			is_online_str = list_entry(socket_status->list.next,struct select_row,list)->argv[0] > 0? "在线":"离线";
			sprintf(send_data,"%s(%s)",friend_name,is_online_str);
			send_msg(socket,LISTING_FRIEND,send_data);
			free_select_rows_list(socket_status);
		}
		send_msg(socket,LISTING_FRIEND_FINISHED,"所有好友都已经发送");
	}
	free_select_rows_list(frow_head);
	return 0;
}