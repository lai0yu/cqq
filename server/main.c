#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// 最大连接数
#define MAX_CONNECT 1000

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

	// 初始化服务器套接字
	int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

	// 服务器套接字绑定地址信息
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
	serv_addr.sin_port = htons(atoi(serv_port));
	bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	// 监听服务器套接字
	listen(serv_sock, MAX_CONNECT);

	// 客户端套接字列表，如果客户端连上了就放在这个列表中
	// TODO：可能要放在数据库上面。
	int client_socks[MAX_CONNECT] = {-1};
	struct sockaddr_in* client_addrs[MAX_CONNECT] = {NULL};
	int ccount = 0;
	fd_set fdset;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// 临时变量得定义放在循环外面，节省栈空间地操作
	// nclient_sock, nc_size
	int i, j, select_ret, recv_ret, nclient_sock = -1, nc_size;
	struct sockaddr_in* nclient_addr = NULL;

	char recv_buf[1284];
	while(1)
	{
		FD_ZERO(&fdset);
		FD_SET(serv_sock, &fdset);
		for(i = 0; i < ccount; i++)
		{
			if(client_socks[i] > 0)
			{
				FD_SET(client_socks[i], &fdset);
			}
		}
		select_ret = select(MAX_CONNECT + 1, &fdset, NULL, NULL, &tv);
		if(select_ret < 0)
		{
			perror("select failed");
			break;
		}
		else if(select_ret == 0)
		{
			continue;
		}
		else
		{
			if(FD_ISSET(serv_sock, &fdset))
			{
				nclient_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
				nclient_sock = accept(serv_sock, (struct sockaddr*)nclient_addr, &nc_size);
				if(nclient_sock > 0)
				{
					if(ccount <= MAX_CONNECT)
					{
						client_addrs[ccount] = nclient_addr;
						client_socks[ccount] = nclient_sock;
						ccount++;
						// TODO:Send Client Connect OK;
						// TODO:Update online table

						printf("A new client %s:%d has connected!\n",
							   inet_ntoa(nclient_addr->sin_addr),
							   ntohs(nclient_addr->sin_port));
					}
					else
					{
						printf("A new client %s:%d connect has been refuse because max "
							   "connect!\n",
							   inet_ntoa(nclient_addr->sin_addr),
							   ntohs(nclient_addr->sin_port));

						// TODO：Send Client message Has Been refuse cause of max connect!

						// send(nclient_sock, refusemsg, sizeof(refusemsg), 1);

						close(nclient_sock);
						free(nclient_addr);
						nclient_addr = NULL;
						nc_size = -1;
					}
				}
			}

			for(int i = 0; i < ccount; i++)
			{
				if(FD_ISSET(client_socks[i], &fdset))
				{
					bzero(recv_buf, sizeof(recv_buf));
					int recv_ret = recv(client_socks[i], recv_buf, sizeof(recv_buf), 0);
					if(recv_ret > 0)
					{
						// TODO:unpack_json

						// TODO:service

						printf("Message from client %s:%d==>%s\n",
							   inet_ntoa(client_addrs[i]->sin_addr),
							   ntohs(client_addrs[i]->sin_port),
							   recv_buf);
					}
				}
			}
		}
	}

	close(serv_sock);
	for(i = 0; i < MAX_CONNECT; i++)
	{
		if(client_addrs[i] != NULL)
		{
			free(client_addrs[i]);
			client_addrs[i] = NULL;
		}
		if(client_socks >= 0)
		{
			close(client_socks[i]);
			client_socks[i] = -1;
		}
	}
	return 0;
}