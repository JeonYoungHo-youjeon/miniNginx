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
	kq->set_server_event(socket, socket->get_fd());

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

	std::cout << "ACCEPT" << std::endl;
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
		switch (state)
		{
		case READ_REQUEST:
			PRINT_LOG("READ_REQUEST");

		// system("leaks webserv");
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
			// req->print_request();
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

	// std::cout << "1\n";
	// system("leaks webserv");
	Request* req = socket->get_request();
	Response* res = socket->get_response();


	// std::cout << "2\n";
	// system("leaks webserv");
	if (state == END_RESPONSE)
	{
		PRINT_LOG("END_RESPONSE");
		if (req->is_empty_buffer() == false)
		{
			PRINT_LOG("IS NOT EMPTY BUFFER");
			socket->update_state(REPEAT_REQUEST);
			handle_client_read_event(socket);
		}
		else if (socket->get_PID())
		{
			std::cout << "state : " << socket->get_state() << std::endl;
			std::cout << "PID : " << socket->get_PID() << std::endl;
			disconnection(socket);
		}
		else if (res->Header["Connection"] == "keep-alive")
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
		PRINT_LOG("SET NEXT EVENT");
		socket->update_state(state);
		kq->set_next_event(socket, socket->get_state());
	}
}

void Event::socket_timeout(const ClientSocket* socket)
{
	// TODO: CGI kill
	if (socket->is_expired() && sockets.count(socket->get_fd()))
	{
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
		system("lsof | grep webserv");
		std::cout << "errno : " << errno << std::endl;
		return; // TODO: response 503 Service Unavailable
	}

	if (event->flags & EV_EOF && socket->get_PID())
	{
		PRINT_LOG("EV_EOF");
		socket->get_response()->TEMP = false;
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
		std::cout << "\twaitpid : " << pid << std::endl;
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