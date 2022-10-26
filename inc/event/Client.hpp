#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>

# include "../http.hpp"

class Client
{
public:
	int get_fd() const;
	int get_server_fd() const;
	const std::string get_ip() const;
	const std::string& get_server_ip() const;
	const std::string& get_server_port() const;
	Request& get_request()
	{
		return (req);
	};

	Client(int clientFd, const struct sockaddr_in& clientAddr, int serverFd,  \
			const std::string& serverIP, const std::string& serverPort);
	~Client();

private:
	Client();
	Client& operator=(const Client& client);
	
private:
	const int clientFd;
	const int serverFd;
	const struct sockaddr_in addr;
	const std::string serverIP;
	const std::string serverPort;
	Request req;
};

// Client implementation

/**
 * @brief 클라이언트의 ip를 반환하는 함수(ex. "0.0.0.0")
 * 
 * @param None
 * 
 * @return ip(string)
*/
const std::string Client::get_ip() const
{
	return inet_ntoa(addr.sin_addr);
}

/**
 * @brief 클라이언트의 fd를 반환하는 함수
 * 
 * @param None
 * 
 * @return fd(int)
*/
int Client::get_fd() const
{
	return clientFd;
}	

/**
 * @brief 통신하는 서버의 fd를 반환하는 함수
 * 
 * @param None
 * 
 * @return fd(int)
*/
int Client::get_server_fd() const
{
	return serverFd;
}

/**
 * @brief 통신하는 서버의 ip를 반환하는 함수(ex. "0.0.0.0")
 * 
 * @param None
 * 
 * @return ip(string)
*/
const std::string& Client::get_server_ip() const
{
	return serverIP;
}

/**
 * @brief 통신하는 서버의 port를 반환하는 함수
 * 
 * @param None
 * 
 * @return port(string)
*/
const std::string& Client::get_server_port() const
{
	return serverPort;
}

/**
 * @brief 클라이언트와 통신하는 서버의 정보를 가지고 있는 클라이언트를 생성하기 위한 생성자
 * 
 * @param clientFd(int) 클라이언트의 fd
 * @param addr(struct sockaddr_in) 클라이언트의 주소 정보가 담긴 구조체
 * @param serverFd(int) 서버의 fd
 * @param serverIp(string) 서버의 ip
 * @param serverPort(string) 서버의 port
 * 
 * @return None
*/
Client::Client(int clientFd, const struct sockaddr_in& clientAddr, \
	int serverFd_, const std::string& serverIp_, const std::string& serverPort_)
	: clientFd(clientFd), addr(clientAddr), \
	serverFd(serverFd_), serverIP(serverIp_), serverPort(serverPort_)
{}

Client::~Client()
{}

// private
Client::Client()
	: clientFd(0), serverFd(0), \
	addr(sockaddr_in{0}), serverIP(0), serverPort(0)
{}

// private
Client& Client::operator=(const Client& client)
{
	return *this;
}

# endif