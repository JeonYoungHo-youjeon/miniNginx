#ifndef SOCKET_HPP
# define SOCKET_HPP


# include "Type.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"

class Socket
{
public:
	FD get_fd() const;
	const String& get_ip() const;
	const String& get_port() const;
	const Type& get_type() const;

	Socket();
	Socket(const String& ip_, const String& port_);
	Socket& operator=(const Socket& socket);
	virtual ~Socket();

protected:
	FD fd;
	String ip;
	String port;
	Type type;
};


// Socket implementation

/**
 * @brief 해당 소켓의 fd를 가져오기 위한 함수
 * 
 * @param None
 * 
 * @return 해당 소켓의 fd(int)
*/
FD Socket::get_fd() const
{
	return fd;
}
/**
 * @brief 해당 소켓의 ip를 가져오기 위한 함수
 * 
 * @param None
 * 
 * @return 해당 소켓의 ip(string)
*/
const String& Socket::get_ip() const
{
	return ip;
}

/**
 * @brief 해당 소켓의 port를 가져오기 위한 함수
 * 
 * @param None
 * 
 * @return 해당 소켓의 port(string)
*/
const String& Socket::get_port() const
{
	return port;
}

const Type& Socket::get_type() const
{
	return type;
}

Socket::Socket()
{}

Socket& Socket::operator=(const Socket& socket)
{
	if (this != &socket)
	{
		this->fd = socket.fd;
		this->ip = socket.ip;
		this->port = socket.port;
	}
	return *this;
}

Socket::~Socket()
{
	close(fd);
}

#endif