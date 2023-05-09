#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lib/json_util/json_util.h"

static int serv_sock;

static char gusername[64];
static char gpassword[64];

int sign_in();
int sign_up();

void login_ui();
void main_ui();

void fun_login();
void fun_main();

int sign_in()
{
	printf("请输入用户名和密码，以空格符分割\n");
	char username[64] = {0};
	char password[64] = {0};

	scanf("%s%s", username, password);

	struct json_object* object = json_object_new_object();

	struct json_object* obj_username = json_object_new_string(username);
	struct json_object* obj_password = json_object_new_string(password);

	json_object_object_add(object, "username", obj_username);
	json_object_object_add(object, "password", obj_password);

	const char* sin_in_data_string = json_object_to_json_string(object);

	struct msg cmsg;
	memset(&cmsg, 0, sizeof(cmsg));
	cmsg.code = 0;
	strcpy(cmsg.cmd, "sign_in");
	strcpy(cmsg.data, sin_in_data_string);

	const char* msg_str = pack_msg(cmsg);

	send(serv_sock, msg_str, sizeof(cmsg), 0);

	char recv_buf[1284];
	bzero(recv_buf, sizeof(recv_buf));
	recv(serv_sock, recv_buf, sizeof(recv_buf), 0);

	struct msg smsg = parse_msg(recv_buf);

	if(smsg.code == 0)
	{
		strcpy(gusername, username);
		strcpy(gpassword, password);
	}
	printf("%s\n", smsg.data);

	return smsg.code;
}

int sign_up()
{
	printf("请输入用户名和密码，以空格符分割\n");
	char username[64] = {0};
	char password[64] = {0};

	scanf("%s%s", username, password);

	struct json_object* object = json_object_new_object();

	struct json_object* obj_username = json_object_new_string(username);
	struct json_object* obj_password = json_object_new_string(password);

	json_object_object_add(object, "username", obj_username);
	json_object_object_add(object, "password", obj_password);

	const char* sin_in_data_string = json_object_to_json_string(object);

	struct msg cmsg;
	memset(&cmsg, 0, sizeof(cmsg));
	cmsg.code = 0;
	strcpy(cmsg.cmd, "sign_up");
	strcpy(cmsg.data, sin_in_data_string);

	const char* msg_str = pack_msg(cmsg);

	send(serv_sock, msg_str, sizeof(cmsg), 0);

	char recv_buf[1284];
	bzero(recv_buf, sizeof(recv_buf));
	recv(serv_sock, recv_buf, sizeof(recv_buf), 0);

	struct msg smsg = parse_msg(recv_buf);

	printf("%s\n", smsg.data);

	return smsg.code;
}

void login_ui()
{
	printf("***************登录界面*****************\n");
	printf("*                                     *\n");
	printf("*              A:登录                  *\n");
	printf("*                                     *\n");
	printf("*              B:注册                  *\n");
	printf("*                                     *\n");
	printf("*              X:退出                  *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void main_ui()
{

	printf("***************功能界面*****************\n");
	printf("*                                     *\n");
	printf("*              A:好友列表              *\n");
	printf("*                                     *\n");
	printf("*              B:群列表                *\n");
	printf("*                                     *\n");
	printf("*              X:退出                  *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void fun_login()
{
	login_ui();
	char c;
	scanf("%c", &c);
	getchar();
	while(c != 'A' && c != 'B' && c != 'X')
	{
		printf("请输入功能选项字母\n");
		scanf("%c", &c);
		getchar();
	}

	if(c == 'A')
	{
		int sign_in_ret = sign_in();
		if(sign_in_ret == 0)
		{
			fun_main();
		}
	}
	else if(c == 'B')
	{
		int sign_up_ret = sign_up();
		if(sign_up_ret == 0)
		{
			printf(" 按#返回登录界面\n");
			char c;
			scanf("%c", &c);
			getchar();
			while(c != '#')
			{
				scanf("%c", &c);
				getchar();
			}

			fun_login();
		}
	}
	else if(c == 'X')
	{
		exit(0);
	}
}

void fun_main()
{
	main_ui();
}

int main(int argc, char* argv[])
{
	// 默认服务器IP 端口
	char serv_ip[128] = "192.168.60.91";
	char serv_port[128] = "54321";

	if(argc == 3)
	{
		bzero(serv_ip, sizeof(serv_ip));
		bzero(serv_port, sizeof(serv_port));
		strcpy(serv_ip, argv[1]);
		strcpy(serv_port, argv[2]);
	}
	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	struct sockaddr_in serv_sock_addr;
	serv_sock_addr.sin_family = AF_INET;
	serv_sock_addr.sin_addr.s_addr = inet_addr(serv_ip);
	serv_sock_addr.sin_port = htons(atoi(serv_port));
	int connect_ret = connect(serv_sock, (struct sockaddr*)&serv_sock_addr, sizeof(serv_sock_addr));
	if(connect_ret < 0)
	{
		perror("connect error");
		return -1;
	}

	fun_login();

	close(serv_sock);
	return 0;
}
