#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
void* recv_work(void* arg)
{
	char buf[1284];
	int recv_ret = -1;
	while(1)
	{
		bzero(buf, sizeof(buf));
		int recv_ret = recv(*(int*)(arg), buf, sizeof(buf), 0);
		if(recv_ret > 0)
		{
			printf("%s\n", buf);
		}
	}
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
	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

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

	pthread_t recv_thread;
	pthread_create(&recv_thread, NULL, recv_work, &serv_sock);
	pthread_detach(recv_thread);

	char buf[1284];
	while(1)
	{
		bzero(buf, sizeof(buf));
		scanf("%s", buf);
		send(serv_sock, buf, strlen(buf), 0);
	}
	pthread_cancel(recv_thread);
	close(serv_sock);
	return 0;
}
