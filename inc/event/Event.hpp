#ifndef EVENT_HPP
# define EVENT_HPP


# include "Type.hpp"
# include "KQueue.hpp"
# include "ClientSocket.hpp"
# include "ServerSocket.hpp"
# include "Logger.hpp" // REMOVE

extern Config g_conf;

class Event
{
public:
	typedef std::map<FD, Socket*>	SocketMap;
	typedef std::vector<const Socket*>	GarbageCollector;
public:
	void event_loop();
	
	Event();
	~Event();

private:
	void create_server_socket(const ConfigType::iterator it);
	void accept_connection(FD serverFD);
	void create_client_socket(FD clientFD, const SockAddr& addr, FD serverFD);
	void disconnection(const ClientSocket* socket);
	void handle_client_read_event(ClientSocket* socket);
	void handle_client_write_event(ClientSocket* socket);
	void handle_next_event(ClientSocket* socket, State state);
	void socket_timeout(const ClientSocket* socket);
	void add_garbage(const Socket* socket);

	void handle_server_event(const KEvent* event, const ServerSocket* socket);
	void handle_client_event(const KEvent* event, ClientSocket* socket);
	void handle_child_process(const KEvent* event, ClientSocket* socket);
	void clear_garbage_sockets();
	void set_next_event(State state);

	Event& operator=(const Event& event);
	Event(const Event& event);

private:
	KQueue* kq;
	SocketMap sockets;
	GarbageCollector garbageCollector;
	Logger logger; // REMOVE
};


void Event::event_loop()
{
	logger.info();

	int nEvent;
	const Socket* socket;
	const KEvent* event;

	while (true)
	{
		try
		{
			nEvent = kq->wait_event();

			for (int i = 0; i < nEvent; ++i)
			{
				event = &(kq->get_eventList()[i]);
				socket = (Socket*)event->udata;

				if (event->filter == EVFILT_PROC)
					handle_child_process(event, (ClientSocket*)socket);
				else if (socket->get_type() == SERVER)
					handle_server_event(event, (ServerSocket*)socket);
				else if (socket->get_type() == CLIENT)
					handle_client_event(event, (ClientSocket*)socket);

				if (!garbageCollector.empty())
					clear_garbage_sockets();
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "\t===== LOOP_ERROR =====" << std::endl;
		}
	}
}

Event::Event()
	: kq(new KQueue())
{
	ConfigType servers = g_conf.getServers();
	
	for (ConfigType::iterator it = servers.begin(); it != servers.end(); ++it)
		create_server_socket(it);
}

Event::~Event()
{
	delete kq;
	for (SocketMap::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
		delete it->second;
}

void Event::create_server_socket(const ConfigType::iterator it)
{
	std::vector<std::string> temp = Util::split(it->first, ':');
	ServerSocket* socket = new ServerSocket(temp[0], temp[1]);

	sockets.insert(std::pair<FD, Socket*>(socket->get_fd(), socket));
	kq->set_server_event(socket, socket->get_fd());

	logger.add_server(socket->get_fd(), it->first); // REMOVE
}

void Event::accept_connection(FD serverFD)
{
	SockAddr clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	FD clientFD = accept(serverFD, (struct sockaddr*)&clientAddr, &clientAddrLen);

	if (clientFD == -1)
	{
		std::cout << "cliendFD : -1" << std::endl;
		std::cout << "errno : " << errno << std::endl;
		return;
	}

	if (fcntl(clientFD, F_SETFL, O_NONBLOCK) == -1)
	{
		ClientSocket* socket = (ClientSocket*)sockets[clientFD];
		State state = socket->set_response(500);
		kq->set_next_event(socket, state);
	}
	
	create_client_socket(clientFD, clientAddr, serverFD);
}

void Event::create_client_socket(FD clientFD, const SockAddr& addr, FD serverFD)
{
	const std::string s = sockets[serverFD]->get_ip() + ":" + sockets[serverFD]->get_port();
	ClientSocket* socket = new ClientSocket(clientFD, addr, s);
	kq->set_client_event(socket, socket->get_fd());
	sockets.insert(std::pair<FD, Socket*>(socket->get_fd(), socket));

	logger.connection_logging(socket, LOG_GREEN); // REMOVE
}

void Event::disconnection(const ClientSocket* socket)
{	
	logger.disconnection_logging(socket, LOG_YELLOW);
	add_garbage(socket);
}

void Event::handle_client_read_event(ClientSocket* socket)
{
	if (socket->is_expired())
		return;

	State state = socket->get_state();
	Request* req = socket->get_request();
	Response* res = socket->get_response();

	try
	{
		switch (state)
		{
		case READ_REQUEST:
			PRINT_LOG("READ_REQUEST");
			state = req->read();
			break;
		case REPEAT_REQUEST:
			PRINT_LOG("REPEAT_REQUEST");
			state = req->clear_read();
			break;
		case READ_RESPONSE:
			PRINT_LOG("READ_RESPONSE");
			state = res->read();
			break;
		}
		if (state == END_REQUEST) {
			PRINT_LOG("END_REQUEST");
			req->print_request();
			state = socket->set_response(*req);
		}
	}
	catch (int error_code)
	{
		PRINT_LOG("CATCH ERROR");
		state = socket->set_response(error_code);
	}

	handle_next_event(socket, state);
}

void Event::handle_client_write_event(ClientSocket* socket)
{
	if (socket->is_expired())
		return;
		
	State state = socket->get_state();
	Response* res = socket->get_response();

	try
	{
		switch (state)
		{
		case WRITE_RESPONSE:
			PRINT_LOG("WRITE_RESPONSE");
			state = res->write();
			break;
		case SEND_RESPONSE:
			PRINT_LOG("SEND_RESPONSE");
			state = res->send(socket->get_fd());
			break;	
		}

	}
	catch (int error_code)
	{
		state = socket->set_response(error_code);
	}
	handle_next_event(socket, state);
}

void Event::handle_next_event(ClientSocket* socket, State state)
{
	Request* req = socket->get_request();
	Response* res = socket->get_response();

	if (state == END_RESPONSE)
	{
		PRINT_LOG("END_RESPONSE");
		// if (req->is_empty_buffer() == false)
		// {
		// 	PRINT_LOG("IS NOT EMPTY BUFFER");
		// 	socket->update_state(REPEAT_REQUEST);
		// 	handle_client_read_event(socket);
		// }
		if (socket->get_PID())
		{
			disconnection(socket);
		}
		else if (res->Header["connection"] == "keep-alive")
		{
			PRINT_LOG("KEEP_ALIVE");
			socket->reset();
			kq->on_read_event(socket, socket->get_fd());
			socket->update_state(READ_REQUEST);
		}
		else
		{
			PRINT_LOG("DISCONNECTION");
			socket->update_state(NOTHING);
			disconnection(socket);
		}
	}
	else
	{
		PRINT_LOG("SET_NEXT_EVENT");
		socket->update_state(state);
		kq->set_next_event(socket, socket->get_state());
	}
}

void Event::socket_timeout(const ClientSocket* socket)
{
	if (socket->is_expired() && sockets.count(socket->get_fd()))
	{
		logger.disconnection_logging(socket, LOG_YELLOW);
		add_garbage(socket);
	}
}

void Event::handle_server_event(const KEvent* event, const ServerSocket* socket)
{
	if (event->flags & EV_ERROR)
		return;

	accept_connection(socket->get_fd());
}

void Event::handle_client_event(const KEvent* event, ClientSocket* socket)
{
	if (event->filter == EVFILT_TIMER)
	{
		PRINT_LOG("EVFILT_TIMER");
		socket_timeout(socket);	
	}
		
	if (event->flags & EV_ERROR)
	{
		PRINT_LOG("EV_ERROR");
		return;
	}

	if (event->flags & EV_EOF && socket->get_PID())
	{
		PRINT_LOG("EV_EOF");
		socket->get_response()->TEMP = false;
		// TODO
		handle_client_read_event(socket);
	}
	else if (event->filter == EVFILT_READ)
	{
		PRINT_LOG("EVFILT_READ");
		handle_client_read_event(socket);
	}
	else if (event->filter == EVFILT_WRITE)
	{
		PRINT_LOG("EVFILT_WRITE");
		handle_client_write_event(socket);
	}
}

void Event::handle_child_process(const KEvent* event, ClientSocket* socket)
{
	PRINT_LOG("EVFILT_PROC");
	if (socket->get_PID() > 0)
	{
		int status;
		PID pid = waitpid(socket->get_PID(), &status, WNOHANG);
	}
}

void Event::clear_garbage_sockets()
{
	PRINT_LOG("CLEAR_GARBASE_SOCKETS");
	for (GarbageCollector::const_iterator it = garbageCollector.begin(); it != garbageCollector.end(); ++it)
	{
		sockets.erase((*it)->get_fd());
		delete (*it);	
	}
	garbageCollector.clear();
}


void Event::add_garbage(const Socket* socket)
{
	garbageCollector.push_back(socket);
}

// private
Event& Event::operator=(const Event& event)
{
	return *this;
}

// private
Event::Event(const Event& event)
{}

#endif