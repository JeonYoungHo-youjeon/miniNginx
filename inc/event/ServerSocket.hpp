#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP


# include "Type.hpp"
# include "Socket.hpp"
# include "../exception/Exception.hpp"


class ServerSocket
	: public Socket
{
public:
	ServerSocket(const String& ip_, const String& port_)
	{
		ip = ip_;
		port = port_;
		type = SERVER;

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
			throw EventInitException("socket() error");
		

		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &ENABLE_REUSERADDR, sizeof(ENABLE_REUSERADDR)) == -1)
			throw EventInitException("setsockopt() error");

		// if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &ENABLE_KEEPALIVE, sizeof(ENABLE_KEEPALIVE)) == -1)
		// 	throw EventInitException("setsockopt() error");

		// TODO: keepalive time 조정

		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
			throw EventInitException("fcntl() error");

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip_.c_str());
		addr.sin_port = htons(std::atoi(port_.c_str()));

		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
			throw EventInitException("bind() error");

		if (listen(fd, SOMAXCONN) == -1)
			throw EventInitException("listen() error");
	}
	ServerSocket(const ServerSocket& other) {};
	// ServerSocket operator=(const ServerSocket& rhs);
	~ServerSocket() {};
private:
	ServerSocket() {};
public:
private:
	static const int ENABLE_REUSERADDR;
	static const int ENABLE_KEEPALIVE;
	SockAddr addr;
};

// setsockopt시 reuseaddr를 위한 설정 상수
const int ServerSocket::ENABLE_REUSERADDR = 1;
const int ServerSocket:: ENABLE_KEEPALIVE = 1;

#endif // SERVERSOCKET_HPP