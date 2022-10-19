#ifndef SOCKET_HPP
# define SOCKET_HPP


# include <sys/socket.h>
# include <netinet/in.h>
# include <cstring>
# include <cstdlib>
# include <fcntl.h>
# include <arpa/inet.h>

# include "../parse/Config.hpp"

class Socket
{
public:
	int get_fd() const;
	const std::string& get_ip() const;
	const std::string& get_port() const;

	Socket(const std::string& ip_, const std::string& port_);
	Socket& operator=(const Socket& socket);
	Socket() {};
	~Socket() {};

protected:

private:

public:

protected:

private:
	int sock_fd;
	std::string ip;
	std::string port;
	struct sockaddr_in addr;
	static const int enable;
};


// Socket implementation

const int Socket::enable = 1;

int Socket::get_fd() const
{
	return sock_fd;
}

const std::string& Socket::get_ip() const
{
	return ip;
}

const std::string& Socket::get_port() const
{
	return port;
}

Socket::Socket(const std::string& ip_, const std::string& port_)
: ip(ip_), port(port_)
{
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
		throw; // FIXME:
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(std::atoi(port_.c_str()));

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	fcntl(sock_fd, F_SETFL, O_NONBLOCK);

	if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cout << "bind error" << std::endl;
		throw; // FIXME:
	}

	if (listen(sock_fd, SOMAXCONN) == -1)
		throw; // FIXME:
}

Socket& Socket::operator=(const Socket& socket)
{
	if (this != &socket)
	{
		sock_fd = socket.get_fd();
		ip = socket.get_ip();
		port = socket.get_port();
		addr = socket.addr;
	}
	return *this;
}


#endif