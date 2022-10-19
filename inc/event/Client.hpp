#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>

class Client
{
public:
	Client(int clientFd, const struct sockaddr_in& clientAddr)
	: sock_fd(clientFd), addr(clientAddr)
	{}

	int get_fd() const
	{
		return sock_fd;
	}
	
	~Client() {};
protected:
private:
	Client();
public:
protected:
private:
	const int sock_fd;
	const struct sockaddr_in addr;
};

# endif