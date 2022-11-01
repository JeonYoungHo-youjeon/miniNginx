#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP


# include "Type.hpp"
# include "Socket.hpp" // TODO: UPDATE

class ClientSocket
	: public Socket
{
public:
	ClientSocket(FD fd_, SockAddr addr_);
	~ClientSocket();

	bool is_expired() const;
	void update_lastEventTime();

private:
	ClientSocket();
	ClientSocket(const ClientSocket& other);
	ClientSocket operator=(const ClientSocket& rhs);

	Time get_current_time() const;

private:
	Time lastEventTime;
};

// ClientSocket implementation

ClientSocket::ClientSocket(FD fd_, SockAddr addr_)
{
	fd = fd_;
	ip = inet_ntoa(addr_.sin_addr);
	port = "";
	type = CLIENT;
	lastEventTime = get_current_time();
}

ClientSocket::~ClientSocket()
{}

bool ClientSocket::is_expired() const
{
	return (get_current_time() - lastEventTime >= TIMEOUT) ? true : false;
}

void ClientSocket::update_lastEventTime()
{
	lastEventTime = get_current_time();
}

// private
ClientSocket::ClientSocket()
{}

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