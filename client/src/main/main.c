#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lib/json_util/json_util.h"

#define SIGN_IN 1
#define SIGN_IN_SUCCESS 2
#define SIGN_IN_NO_USER 3
#define SIGN_IN_PW_ERROR 4

#define SIGN_OUT 11
#define SIGN_OUT_SUCCESS 12

#define SIGN_UP 21
#define SIGN_UP_SUCCESS 22
#define SIGN_UP_DUP_USER 23

#define SIGN_DEL 31
#define SIGN_DEL_SUCCESS 32
#define SIGN_DEL_NO_USER 33
#define SIGN_DEL_PW_ERROR 34

static int serv_sock;

static char gusername[64];
static char gpassword[64];

int sign_in();
int sign_up();

void login_ui();
void main_ui();

void fun_login();
void fun_main();

static int send_msg(int socket, char code, char* data) {
	struct msg smsg;
	memset(&smsg, 0, sizeof(struct msg));
	smsg.code = code;
	strcpy(smsg.data, data);

	char msg_buf[1024] = { 0 };
	pack_msg(smsg, msg_buf);
	return send(socket, msg_buf, strlen(msg_buf) + 1, 0);
	
}

int sign(char code)
{
	struct sign_data sd;
	memset(&sd,0,sizeof(struct sign_data));
	
	printf("请输入用户名:\n");
	scanf("%s" ,sd.username);

	printf("请输入密码:\n");
	scanf("%s" ,sd.password);

	char psd[1023];
	pack_signdata(sd,psd);
	return send_msg(serv_sock, code, psd);
}


void login_ui()
{
	printf("***************登录界面*****************\n");
	printf("*                                     *\n");
	printf("*              1:登录                  *\n");
	printf("*                                     *\n");
	printf("*              2:注册                  *\n");
	printf("*                                     *\n");
	printf("*              3:退出                  *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void main_ui()
{
	printf("***************功能界面*****************\n");
	printf("*                                     *\n");
	printf("*              1:好友列表              *\n");
	printf("*                                     *\n");
	printf("*              2:群列表                *\n");
	printf("*                                     *\n");
	printf("*              3:退出                  *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void fun_login()
{
	unsigned char c;
	scanf("%hhd",&c);

	while(c <1 || c>3){
		scanf("%hhd",&c);
	}

	switch (c)
	{
		case 1:sign(SIGN_IN);break;
		case 2:sign(SIGN_UP);break;
		case 3:exit(-1);break;
		default:
			break;
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
