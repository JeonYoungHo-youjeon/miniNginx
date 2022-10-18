#include "../../inc/http.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// 소켓을 만들어서 클라에서 연결을 받아서 accept 한다.
// 클라와 연결된 소켓에서 데이터를 받아서 Request 구조체에 담는다
// 데이터를 전부 받으면 Request 데이터를 콘솔에 출력한다.
// 미리 만들어둔 Response 구조체에서 데이터를 받아서 클라에 전달한다.
int main()
{
	int s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	uint16_t port = 8000;
	uint32_t address = INADDR_ANY;
	int optval = 1;
	setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = address;

	if (bind(s_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(s_fd);
		throw std::runtime_error(
			std::string("bind() failed: ") +
			strerror(errno)
		);
	}
	if (listen(s_fd, 1024) < 0) {
		close(s_fd);
		throw std::runtime_error(
			std::string("listen() failed: ") +
			strerror(errno)
		);
	}

	struct sockaddr_in c_addr;
	socklen_t len = sizeof(c_addr);
	int c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &len);
	if (c_fd < 0)
	{
		throw std::runtime_error(
			std::string("accept() failed: ") +
			strerror(errno)
		);
	}
	Request req;

	while (true)
	{
		char buf[1024] = {};
		int n = read(c_fd, buf, 1024);
		std::string str = buf;

		if (n <= 0)
		{
			if (n < 0)
				std::cerr << "client read error!" << endl;
			std::cout << "close input" << std::endl;
			break ;
		}

		printf("Client Msg = %s\n", buf);
		cout << str << endl;
		
		try
		{
			req.set_request(str);
		}
		catch (...)
		{
			std::cerr << "TEST" << endl;
			break ;
		}
	}

	req.print_request();

	Response res;

	res.protocol = "HTTP/1.1";	
	res.statusCode = "200";	
	res.contentLength = "12";	
	res.connection = "close";	
	res.server = "webserv";
	res.body = req.body;
	res.print_response();
	std::cout << res.get_response().c_str() << std::endl;
	send(c_fd, res.get_response().c_str(), res.get_response().size(), 0);

	close(c_fd);


}