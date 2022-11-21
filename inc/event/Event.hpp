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
	void handle_client_event(const KEvent* event, const ClientSocket* socket);
	void handle_child_process(const KEvent* event);
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

// Event implementation

/**
 * @brief connection, read, write, disconnection 등의 event 발생을 감시하며,
 * 			event 발생시 알려주는 infinite loop
 * 
 * @param None
 * 
 * @return None
*/
void Event::event_loop()
{
	logger.info(); // REMOVE

	int nEvent;
	const Socket* socket;
	const KEvent* event;

	while (true)
	{
		nEvent = kq->wait_event();

		for (int i = 0; i < nEvent; ++i)
		{
			event = &(kq->get_eventList()[i]);
			socket = (Socket*)event->udata;

			if (socket->get_type() == SERVER)
				handle_server_event(event, (ServerSocket*)socket);
			else if (socket->get_type() == CLIENT)
				handle_client_event(event, (ClientSocket*)socket);
			else if (event->filter == EVFILT_PROC)
				handle_child_process(event);

			if (!garbageCollector.empty())
				clear_garbage_sockets();
		}
	}
}

/**
 * @brief config 설정에 따라 서버의 소켓과 multiplexing을 위한 kqueue를 초기화하는 생성자
 * 
 * @param None
 * 
 * @return None
*/
Event::Event()
	: kq(new KQueue())
{
	ConfigType servers = g_conf.getServers();
	
	for (ConfigType::iterator it = servers.begin(); it != servers.end(); ++it)
		create_server_socket(it);
}

/**
 * @brief 동적 할당받은 객체를 지워주는 소멸자
 * 
 * @param None
 * 
 * @return None
*/
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
	kq->add_read_event(socket, socket->get_fd());

	logger.add_server(socket->get_fd(), it->first); // REMOVE
}

/**
 * @brief 클라이언트의 연결을 받아 등록시키는 함수
 * 
 * @param serverFD(int) 클라이언트가 연결을 시도한 서버의 fd
 * 
 * @return None
*/
void Event::accept_connection(FD serverFD)
{
	SockAddr clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	FD clientFD = accept(serverFD, (struct sockaddr*)&clientAddr, &clientAddrLen);

	if (clientFD == -1)
		return;
	
	create_client_socket(clientFD, clientAddr, serverFD);

	if (fcntl(clientFD, F_SETFL, O_NONBLOCK) == -1)
	{
		// TODO: response 500 Internal Server Error 
		ClientSocket* socket = (ClientSocket*)sockets[clientFD];
		State state = socket->set_response(500);
		kq->set_next_event(socket, state);
	}
}

void Event::create_client_socket(FD clientFD, const SockAddr& addr, FD serverFD)
{
	const std::string s = sockets[serverFD]->get_ip() + ":" + sockets[serverFD]->get_port();
	ClientSocket* socket = new ClientSocket(clientFD, addr, s);
	kq->add_client_io_event(socket, socket->get_fd());
	sockets.insert(std::pair<FD, Socket*>(socket->get_fd(), socket));

	logger.connection_logging(socket, LOG_GREEN); // REMOVE
}

/**
 * @brief 클라이언트와의 연결을 끊는 함수
 * 
 * @param client(Client*) 연결을 끊을 클라이언트
 * 
 * @return None
*/
void Event::disconnection(const ClientSocket* socket)
{	
	logger.disconnection_logging(socket, LOG_YELLOW);
	add_garbage(socket);
}

/**
 * @brief read event 발생 시 클라이언트의 메시지를 수신하는 함수
 * 
 * @param client(Client*) read event를 발생시킨 client
 * 
 * @return None
*/
void Event::handle_client_read_event(ClientSocket* socket)
{
	if (socket->is_expired())
		return;

	State state = socket->get_state();
	Request* req = socket->get_request();
	Response* res = socket->get_response();

	try
	{
		if (state == READ_REQUEST)
		{
			std::cout << "\t==========[READ_REQUEST]==========" << std::endl;
			state = req->read();
		}
		else if (state == REPEAT_REQUEST)
		{
			std::cout << "\t==========[REPEAT_REQUEST]==========" << std::endl;
			state = req->clear_read();
		}
		else if (state == READ_RESPONSE)
		{
			std::cout << "\t==========[READ_RESPONSE]==========" << std::endl;
			state = res->read();
		}

		if (state == END_REQUEST) {
			std::cout << "\t==========[END_REQUEST]==========" << std::endl;
			req->print_request();
			state = socket->set_response(*req);
			std::cout << "state : " << state << std::endl;
		}
		
	}
	catch (int error_code)
	{
		std::cout << "\t==========[CATCH ERROR_CODE : " << error_code << "]==========" << std::endl;
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
		state = res->write();
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

	if (state == SEND_RESPONSE)
	{
		std::cout << "\t==========[SEND_RESPONSE]==========" << std::endl;
		state = res->send(socket->get_fd());
		// send(socket->get_fd(), res->toHtml().c_str(), res->toHtml().size(), 0);

		if (req->is_empty_buffer() == false)
		{
			std::cout << "\t==========[IS NOT EMPTY BUFFER]==========" << std::endl;
			socket->update_state(REPEAT_REQUEST);
			handle_client_read_event(socket);
		}
		else if (res->Header["Connection"] == "keep-alive")
		{
			std::cout << "\t==========[KEEP_ALIVE]==========" << std::endl;
			socket->reset();
			socket->update_state(READ_REQUEST);
			kq->enable_read_event(socket, socket->get_fd());
		}
		else
		{
			std::cout << "\t==========[DISCONNECTION]==========" << std::endl;
			disconnection(socket);
		}
	}
	else
	{
		socket->update_state(state);
		kq->set_next_event(socket, socket->get_state());
	}
}



void Event::socket_timeout(const ClientSocket* socket)
{
	// TODO: CGI kill
	if (socket->is_expired() && sockets.count(socket->get_fd())) {
		logger.disconnection_logging(socket, LOG_YELLOW);
		add_garbage(socket);
	}
}

void Event::handle_server_event(const KEvent* event, const ServerSocket* socket)
{
	// 서버를 종료시키면 모든 client 500 response
	if (event->flags & EV_ERROR)
		return; // TODO: response 500 Internal Server Error

	accept_connection(socket->get_fd());
}

void Event::handle_client_event(const KEvent* event, const ClientSocket* socket)
{
	if (event->filter == EVFILT_TIMER)
	{
		std::cout << "\t==========[EVFILT_TIMER]==========" << std::endl;

		socket_timeout((ClientSocket*)socket);	
	}
		
	if (event->flags & EV_ERROR)
	{
		std::cout << "\t==========[EV_ERROR]==========" << std::endl;
		system("lsof | grep www/");

		std::cout << event->data << std::endl;
		std::cout << socket->get_response()->contentResult->inFd << std::endl;
		std::cout << socket->get_response()->contentResult->outFd << std::endl;
		std::cout << socket->get_readFD() << std::endl;
		std::cout << socket->get_writeFD() << std::endl;
		return; // TODO: response 503 Service Unavailable
	}

	if (event->flags & EV_EOF)
	{
		std::cout << "\t==========[EV_EOF]==========" << std::endl;

		// TODO: I don't know to response anything message
		// disconnection((ClientSocket*)socket);
	}
	else if (event->filter == EVFILT_READ)
	{
		std::cout << "\t==========[EVFILT_READ]==========" << std::endl;

		handle_client_read_event((ClientSocket*)socket);
	}
	else if (event->filter == EVFILT_WRITE)
	{
		std::cout << "\t==========[EVFILT_WRITE]==========" << std::endl;
		handle_client_write_event((ClientSocket*)socket);
	}
}

void Event::handle_child_process(const KEvent* event)
{
	int status;
	PID pid = wait(&status);
	if (WIFEXITED(status))
		std::cout << WEXITSTATUS(status) << std::endl;
}

void Event::clear_garbage_sockets()
{
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