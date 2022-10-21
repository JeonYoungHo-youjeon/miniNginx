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

	Socket();
	Socket(const std::string& ip_, const std::string& port_);
	Socket& operator=(const Socket& socket);
	~Socket();


private:
	int sock_fd;
	std::string ip;
	std::string port;
	struct sockaddr_in addr;
	static const int enable;
};


// Socket implementation

// setsockopt시 reuseaddr를 위한 설정 상수
const int Socket::enable = 1;

/**
 * @brief 해당 소켓의 fd를 가져오기 위한 함수
 * 
 * @param None
 * 
 * @return 해당 소켓의 fd(int)
*/
int Socket::get_fd() const
{
	return sock_fd;
}
/**
 * @brief 해당 소켓의 ip를 가져오기 위한 함수
 * 
 * @param None
 * 
 * @return 해당 소켓의 ip(string)
*/
const std::string& Socket::get_ip() const
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
const std::string& Socket::get_port() const
{
	return port;
}

Socket::Socket()
{}

/**
 * @brief ip와 port로 해당 소켓을 생성하기 위한 생성자
 *  - 소켓 생성
 *  - 해당 소켓이 wait time시 해당 ip와 port를 재사용하기 위한 설정
 *  - fd Non-blocking 처리
 *  - ip, port bind
 *  - listen
 * 
 * @param ip_(string) 소켓에서 사용할 ip
 * @param port_(string) 소켓에서 사용할 port
 * 
 * @return None
*/
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
		this->sock_fd = socket.sock_fd;
		this->ip = socket.ip;
		this->port = socket.port;
		this->addr = socket.addr;
	}
	return *this;
}

Socket::~Socket()
{}

#endif