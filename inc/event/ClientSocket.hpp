#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP


# include "Type.hpp"
# include "Socket.hpp"
# include "Session.hpp"


class ClientSocket
	: public Socket
{
public:
	bool is_expired() const;
	void update_lastEventTime();
	void update_state(State s);
	State set_response(const Request& req);
	State set_response(int error_code);
	void set_readFD(FD fd);
	void set_writeFD(FD fd);
	void set_PID(PID pid);
	FD get_readFD() const;
	FD get_writeFD() const;
	PID get_PID() const;
	Request* get_request() const;
	Response* get_response() const;
	const std::string& get_server_ip_port() const;
	State get_state() const;
	void reset();

	ClientSocket(FD clientFD, const SockAddr& addr, const std::string& serverIPPort_, Session* ses);
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
	FD readFD;
	FD writeFD;
	PID childPID;
	State state;
	Session *session;
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
State ClientSocket::set_response(const Request& req)
{
	return res->set(req);
}

State ClientSocket::set_response(int error_code)
{
	return res->set(req->configName, error_code);
}

void ClientSocket::set_readFD(FD fd)
{
	readFD = fd;
}

void ClientSocket::set_writeFD(FD fd)
{
	writeFD = fd;
}

void ClientSocket::set_PID(PID pid)
{
	childPID = pid;
}

FD ClientSocket::get_readFD() const 
{
	return readFD;
}

FD ClientSocket::get_writeFD() const
{
	return writeFD;
}

PID ClientSocket::get_PID() const
{
	return childPID;
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

void ClientSocket::reset()
{
	Request* tmp_req = req;

	req = new Request(fd, serverIPPort, session);

	delete tmp_req;
}


// private
ClientSocket::ClientSocket()
{}

ClientSocket::ClientSocket(FD clientFD, const SockAddr& addr, const std::string& serverIPPort_, Session* ses)
	: lastEventTime(get_current_time()), serverIPPort(serverIPPort_), \
		readFD(0), writeFD(0), childPID(0), state(READ_REQUEST), req(0), res(new Response())
{
	fd = clientFD;
	ip = inet_ntoa(addr.sin_addr);
	port = "";
	type = CLIENT;
	session = ses;
	req = new Request(fd, serverIPPort, session);
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