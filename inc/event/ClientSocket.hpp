#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP


# include "Type.hpp"
# include "Socket.hpp"

struct ClientFD
{
	ClientFD()
		: readFileFD(0), readCGIFD(0), writeFileFD(0), writeCGIFD(0)
	{}

	FD readFileFD;
	FD readCGIFD;
	FD writeFileFD;
	FD writeCGIFD;
};

class ClientSocket
	: public Socket
{
public:
	bool is_expired() const;
	void update_lastEventTime();
	void update_state(State s);
	void set_response(const Request* req);
	Request* get_request() const;
	Response* get_response() const;
	const std::string& get_server_ip_port() const;
	State get_state() const;

	ClientSocket(FD clientFD, const SockAddr& addr, const std::string& serverIPPort);
	~ClientSocket();

private:
	ClientSocket();
	ClientSocket(const ClientSocket& other);
	ClientSocket operator=(const ClientSocket& rhs);

	Time get_current_time() const;

private:
	Time lastEventTime;
	std::string serverIPPort;
	Request* req;
	Response* res;
	ClientFD* clientFD;
	PID childPID;
	State state;
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

void ClientSocket::update_state(State s)
{
	state = s;
}
void ClientSocket::set_response(const Request* req)
{
	res = new Response(*req);
}

Request* ClientSocket::get_request() const
{
	return req;
}

Response* ClientSocket::get_response() const
{
	return res;
}

const std::string& ClientSocket::get_server_ip_port() const
{
	return serverIPPort;
}

State ClientSocket::get_state() const
{
	return state;
}

// private
ClientSocket::ClientSocket()
{}

ClientSocket::ClientSocket(FD clientFD, const SockAddr& addr, const std::string& serverIPPort_)
	: lastEventTime(get_current_time()), serverIPPort(serverIPPort_), res(NULL), \
		clientFD(new ClientFD()), childPID(0), state(READ_REQUEST),
{
	fd = clientFD;
	ip = inet_ntoa(addr.sin_addr);
	port = "";
	type = CLIENT;
	req = new Request(fd, serverIPPort);
}

ClientSocket::~ClientSocket()
{
	if (req)
		delete req;
	if (res)
		delete res;
	if (clientFD)
		delete clientFD;
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