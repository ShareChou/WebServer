#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdio>
#include <string>

#define PORT 8888
#define MESSAGE_LEN 1024

int main(int argc, char* argv[])
{
	int ret = -1;
	char sendbuf[MESSAGE_LEN] = { 0 };
	char recvbuf[MESSAGE_LEN] = { 0 };

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		std::cout << "Failed to create socket!" << std::endl;
		exit(-1);
	}

	//向服务器（特定的IP和端口）发起请求
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = AF_INET;  //使用IPv4地址
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
	serv_addr.sin_port = PORT;  //端口
	ret = connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (socket_fd < 0) {
		std::cout << "Failed to connect server!" << std::endl;
		exit(-1);
	}

	while (1) {
		memset(sendbuf, 0, MESSAGE_LEN);
		std::cin >> sendbuf;
		ret = write(socket_fd, sendbuf, strlen(sendbuf));
		if (socket_fd <= 0) {
			std::cout << "Failed to connect send data!" << std::endl;
			break;
		}

		if (strcmp(sendbuf, "quit") == 0) {
			break;
		}

		ret = read(socket_fd, recvbuf, MESSAGE_LEN);
		recvbuf[ret] = '\0';
		std::cout << "recv:" << recvbuf << std::endl;
	}

	close(socket_fd);
}