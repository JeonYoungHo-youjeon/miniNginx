#ifndef EVENT_HPP
# define EVENT_HPP


# include "Socket.hpp"
# include "ServerSocket.hpp"
# include "ClientSocket.hpp"
# include "KQueue.hpp"
# include "Type.hpp"
# include "../parse/Util.hpp"
# include "../http.hpp"
# include "../exception/Exception.hpp"
# include "Logger.hpp" // REMOVE

extern Config g_conf;

class Event
{
public:
	typedef std::map<FD, const Socket*>	SocketMap;
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
	void recv_from_client(const ClientSocket* socket);
	void send_to_client(const ClientSocket* socket);
	void socket_timeout(const ClientSocket* socket);
	void add_garbage(const Socket* socket);

	void handle_server_event(const KEvent* event, const ServerSocket* socket);
	void handle_client_event(const KEvent* event, const ClientSocket* socket);
	void handle_child_process(const KEvent* event);
	void clear_garbage_sockets();

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
	const ServerSocket* socket = new ServerSocket(temp[0], temp[1]);

	sockets.insert(std::pair<FD, const Socket*>(socket->get_fd(), socket));
	kq->add_server_io_event(socket);

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
	
	if (fcntl(clientFD, F_SETFL, O_NONBLOCK) == -1)
	{
		// TODO: response 500 Internal Server Error 
		throw EventLoopException("fcntl() error");
	}

	create_client_socket(clientFD, clientAddr, serverFD);
}

void Event::create_client_socket(FD clientFD, const SockAddr& addr, FD serverFD)
{
	std::string s = sockets[serverFD]->get_ip() + ":" + sockets[serverFD]->get_port();
	const ClientSocket* socket = new ClientSocket(clientFD, addr, s);
	kq->add_client_io_event(socket);
	sockets.insert(std::pair<FD, const Socket*>(socket->get_fd(), socket));

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
// TODO : EOF 받을 시 DLE(Data Link Escape) 처리
void Event::recv_from_client(const ClientSocket* socket)
{
	if (socket->is_expired())
		return;


	socket->get_request()->set_request(socket->get_fd(), socket->get_ip_port());
	kq->enable_write_event(socket);
	// if (client->get_request().set_request(client->get_fd(), client->get_ip()))
	// {
	// 	update_event(clientFD, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	// }
	// else 
	// {
	// 	update_event(clientFD, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	// 	update_event(clientFD, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	// }
	// int pid = fork();
	// if (pid) {
	// 	std::cout << "parent process. child pid : " << pid << std::endl;
	// 	kq->add_proc_event(pid);
	// } else {
	// 	std::cout << "child exit" << std::endl;
	// 	exit(33);
	// }
}

/**
 * @brief write event 발생 시 클라이언트에게 메시지를 전송하는 함수
 * 
 * @param client(Client*) write event를 발생시킨 client
 * 
 * @return None
*/
void Event::send_to_client(const ClientSocket* socket)
{
	if (socket->is_expired())
		return;
		
	FD clientFD = socket->get_fd();

	// TODO : request와 response 구현 이후 작성
	int send_len = send(clientFD, "success send_to_client", strlen("success send_to_client"), 0);
	std::cout << "send_len : " << send_len << std::endl;
	kq->enable_read_event(socket);
	// end
}

void Event::socket_timeout(const ClientSocket* socket)
{
	if (socket->is_expired()) {
		logger.disconnection_logging(socket, LOG_YELLOW);
		add_garbage(socket);
	}
}

void Event::handle_server_event(const KEvent* event, const ServerSocket* socket)
{
	// TODO: 서버 하나가 죽으면 서버를 종료시킬지? or add garbage
	// 서버를 종료시키면 모든 clientdprp 500 response
	if (event->flags & EV_ERROR)
		return; // TODO: response 500 Internal Server Error

	accept_connection(socket->get_fd());
}

void Event::handle_client_event(const KEvent* event, const ClientSocket* socket)
{
	if (event->filter == EVFILT_TIMER)
		socket_timeout((ClientSocket*)socket);				
	if (event->flags & EV_ERROR)
		return; // TODO: response 503 Service Unavailable

	if (event->flags & EV_EOF)
	{
		std::cout << "EV_EOF" << std::endl;
		// TODO: I don't know to response anything message
		disconnection((ClientSocket*)socket);
	}
	else if (event->filter == EVFILT_READ)
	{
		std::cout << "EVFILT_RECV" << std::endl;
		recv_from_client((ClientSocket*)socket);
	}
	else if (event->filter == EVFILT_WRITE)
	{
		std::cout << "EVFILT_WRITE" << std::endl;
		send_to_client((ClientSocket*)socket);
	}
}

void Event::handle_child_process(const KEvent* event)
{
	int status;
	pid_t pid = wait(&status);
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