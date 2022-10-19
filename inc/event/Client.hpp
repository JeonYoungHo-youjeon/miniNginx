#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>

class Client
{
public:
	int get_fd() const;
	int get_server_fd() const;
	const std::string get_ip() const;
	const std::string& get_server_ip() const;
	const std::string& get_server_port() const;

	Client(int clientFd, int serverFd, const struct sockaddr_in& clientAddr, \
			const std::string& serverIP, const std::string& serverPort);
	~Client() {};
protected:
private:
	Client();
	
public:
protected:
private:
	const int clientFd;
	const int serverFd;
	const struct sockaddr_in addr;
	const std::string serverIP;
	const std::string serverPort;
};

// Client implementation

const std::string Client::get_ip() const
{
	return inet_ntoa(addr.sin_addr);
}

int Client::get_fd() const
{
	return clientFd;
}	

int Client::get_server_fd() const
{
	return serverFd;
}

const std::string& Client::get_server_ip() const
{
	return serverIP;
}

const std::string& Client::get_server_port() const
{
	return serverPort;
}


Client::Client(int clientFd, int serverFd_, \
			const struct sockaddr_in& clientAddr, \
			const std::string& serverIp_, const std::string& serverPort_)
: clientFd(clientFd), serverFd(serverFd_), \
	addr(clientAddr), serverIP(serverIp_), serverPort(serverPort_)
{}


# endif