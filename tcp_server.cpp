#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888
#define MESSAGE_LEN 1024

int main(int argc, char *argv[])
{
	int socket_fd;
	int ret = -1;
	int on = 1;
	int backlog = 10;
	struct sockaddr_in serv_addr, clnt_addr;
	int accept_fd;
	char in_buff[MESSAGE_LEN] = { 0 };

	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cout << "failed to create socket!" << std::endl;
		exit(1);
	}
	ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // http://c.biancheng.net/cpp/html/374.html
	if (ret == -1) {
		std::cout << "failed to set socket options!" << std::endl;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = PORT;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	ret = bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		std::cout << "failed to bind addr!" << std::endl;
		exit(-1);
	}

	ret = listen(socket_fd, backlog);
	if (ret == -1) {
		std::cout << "failed to listen socket!" << std::endl;
		exit(-1);
	}

	while (1) {
		socklen_t clnt_addr_size = sizeof(clnt_addr);
		accept_fd = accept(socket_fd, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		while (true) {
			ret = read(accept_fd, in_buff, MESSAGE_LEN);
			if (ret = 0) {
				break;
			}
			std::cout << "read:" << in_buff << std::endl;
			write(accept_fd, in_buff, MESSAGE_LEN);
		}
		close(accept_fd);
	}
	close(socket_fd);
	return 0;
}