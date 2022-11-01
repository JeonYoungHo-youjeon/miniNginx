#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP


# include "Type.hpp"
# include "Socket.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"

class ClientSocket
	: public Socket
{
public:
	bool is_expired() const;
	void update_lastEventTime();
	Request* get_request() const;
	const std::string& get_ip_port() const;

	ClientSocket(FD clientFD, const SockAddr& addr, const std::string& ipPort);
	~ClientSocket();

private:
	ClientSocket();
	ClientSocket(const ClientSocket& other);
	ClientSocket operator=(const ClientSocket& rhs);

	Time get_current_time() const;

private:
	Time lastEventTime;
	std::string ipPort;
	Request* req;
	Response* res;
};

// ClientSocket implementation

bool ClientSocket::is_expired() const
{
	return (get_current_time() - lastEventTime >= TIMEOUT) ? true : false;
}

void ClientSocket::update_lastEventTime()
{
	lastEventTime = get_current_time();
}

Request* ClientSocket::get_request() const
{
	return req;
}

const std::string& ClientSocket::get_ip_port() const
{
	return ipPort;
}


// private
ClientSocket::ClientSocket()
{}

ClientSocket::ClientSocket(FD clientFD, const SockAddr& addr, const std::string& ipPort_)
	: lastEventTime(get_current_time()), ipPort(ipPort_), req(new Request()), res(NULL)
{
	fd = clientFD;
	ip = inet_ntoa(addr.sin_addr);
	port = "";
	type = CLIENT;
}

ClientSocket::~ClientSocket()
{
	if (req)
		delete req;
	if (res)
		delete res;
}

//private
ClientSocket::ClientSocket(const ClientSocket& other)
{}

//private
ClientSocket ClientSocket::operator=(const ClientSocket& rhs)
{
	return *this;
}

Time ClientSocket::get_current_time() const
{
	return time(NULL);
}

#endif // CLIENTSOCKET_HPP