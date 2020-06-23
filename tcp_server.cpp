#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>

//#define PORT 8888
#define MESSAGE_LEN 1024
#define MAX_EVENTS 20
#define TIMEOUT 500

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		printf("usage: %s ip_address port_number\n", basename(argv[0]));
		return 1;
	}

	int port = atoi(argv[1]);
	
	int ret = -1;
	int on = 1;
	
	int accept_fd;
	char in_buff[MESSAGE_LEN] = { 0 };

	int event_number;
	int flags = 1; 

	int socket_fd;
	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cout << "failed to create socket!" << std::endl;
		exit(1);
	}

	flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);	//定义成非阻塞
	//set options of socket
	ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // 参考http://c.biancheng.net/cpp/html/374.html
	if (ret == -1) {
		std::cout << "failed to set socket options!" << std::endl;
	}

	struct sockaddr_in serv_addr, clnt_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//serv_addr.sin_port = PORT;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		std::cout << "failed to bind addr!" << std::endl;
		exit(-1);
	}

	int backlog = 10;
	ret = listen(socket_fd, backlog);
	if (ret == -1) {
		std::cout << "failed to listen socket!" << std::endl;
		exit(-1);
	}

	int epoll_fd;
	epoll_fd = epoll_create(5);

	//将侦听的socket放进epoll事件中，这一块可单独写个函数
	struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN;
	ev.data.fd = socket_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

	while (1) {
		event_number = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		for (int i = 0; i < event_number; i++) {
			if (events[i].data.fd == socket_fd) {
				socklen_t clnt_addr_size = sizeof(clnt_addr);
				accept_fd = accept(socket_fd, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

				flags = fcntl(accept_fd, F_GETFL, 0);
				fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);	//定义成非阻塞

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = accept_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
			}
			else if (events[i].events & EPOLLIN) {	//如果是输入事件
				do {
					memset(in_buff, 0, MESSAGE_LEN);
					ret = read(events[i].data.fd, in_buff, MESSAGE_LEN);
					if (ret == 0) {
						close(events[i].data.fd);
					}

					//缓冲区满了
					if (ret == MESSAGE_LEN) {
						std::cout << "maybe have data ..." << std::endl;
					}
				} while (ret < 0 && errno == EINTR);
				

				if (ret < 0) {
					switch (errno) {
					case EAGAIN:
						break;
					default:
						break;
					}
				}

				if (ret > 0) {
					std::cout << "reveive message:" << std::endl;
					write(events[i].data.fd, in_buff, MESSAGE_LEN);
				}
			}
		}
		close(accept_fd);
	}
	close(socket_fd);
	return 0;
}