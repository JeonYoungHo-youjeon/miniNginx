#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
int main()
{
	int fd;
	for (int i = 0; ; ++i) {
		fd = open("./server", O_RDONLY);
		std::cout << fd << std::endl;
		if (fd == -1) {
			std::cout << "exit" << std::endl;
			return 1;
		}
	}
	// int fd = socket(PF_INET, SOCK_STREAM, 0);

	// struct sockaddr_in addr;
	// addr.sin_family = AF_INET;
	// addr.sin_addr.s_addr = INADDR_ANY;
	// addr.sin_port = htons(8080);

	// int enable = 1;
	// setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	// struct timeval t;
	// t.tv_sec = 2;
	// t.tv_usec = 0;
	// if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	// 	std::cerr << "bind() error" << std::endl;
	// 	return (1);
	// }
	// if (listen(fd, 5) == -1) {
	// 	std::cerr << "listen() error" << std::endl;
	// 	return (1);
	// }
	// struct sockaddr_in c_addr;
	// socklen_t len = sizeof(c_addr);
	// int c_fd = accept(fd, (struct sockaddr*)&c_addr, &len);
	// std::cout << "accept" << std::endl;
	// setsockopt(c_fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));

	// char buffer[1024];
	// int size = recv(c_fd, buffer, 1024, 0);
	// std::cout << size << std::endl;
	// std::cout << errno << std::endl;
}