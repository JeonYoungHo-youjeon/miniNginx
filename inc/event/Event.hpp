#ifndef EVENT_HPP
# define EVENT_HPP


# include <vector>
# include <map>
# include <string>
# include <sys/types.h>
# include <sys/event.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>

# include "Socket.hpp"
# include "Client.hpp"
# include "KQueue.hpp"
# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../http.hpp"
# include "../exception/Exception.hpp"
# include "Logger.hpp" // REMOVE

extern Config g_config;

class Event
{
public:
	typedef std::map<std::string, Server>	configType;
	typedef std::map<int, Socket> 			ServerSocketMap;
	typedef std::map<int, Client*>			ClientMap;


public:
	void event_loop();
	
	Event();
	~Event();

private:
	void init_kqueue();
	void accept_connection(int serverFd);
	void register_client(int clientFd, struct sockaddr_in clientAddr, int serverFd);
	void disconnection(Client* client);
	void send_to_client(Client* client);
	void recv_from_client(Client* client);

	Event& operator=(const Event& event);

private:
	ServerSocketMap mServerSocket;
	ClientMap mClient;
	KQueue* kq;
	static const int MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
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
	logger.info();
	int nEvent, currentFd;
	const KQueue::KEvent* currentEvent;

	while (true)
	{
		nEvent = kq->wait_event();
		
		if (nEvent == -1)
			return; // TODO: response 500 Internal Server Error

		for (int i = 0; i < nEvent; ++i)
		{
			currentEvent = &(kq->get_eventList()[i]);
			currentFd = currentEvent->ident;
			if (currentEvent->filter == EVFILT_TIMER) {
				time_t now = time(0);
				char* dt = ctime(&now);
				std::cout << dt;
				std::cout << currentEvent->data << std::endl;
				std::cout << "EVFILT_TIMER : " << currentEvent->ident << std::endl;
			}
			if (currentEvent->flags & EV_ERROR)
			{
				if (mServerSocket.count(currentFd))
					return; // TODO: response 500 Internal Server Error
				else
					return; // TODO: response 503 Service Unavailable
			}
			else if (mServerSocket.count(currentFd))
				accept_connection(currentFd);
			else if (currentEvent->filter == EVFILT_PROC)
			{
				int status;
				pid_t pid = wait(&status);
				if (WIFEXITED(status))
					std::cout << WEXITSTATUS(status) << std::endl;
			}
			else
			{
				if (currentEvent->flags & EV_EOF) // TODO: 필요한 지 확인
					disconnection(mClient[currentFd]);
				else if (currentEvent->filter == EVFILT_READ)
					recv_from_client(mClient[currentFd]);
				else if (currentEvent->filter == EVFILT_WRITE)
				{
					std::cout << "EVFILT_WRITE" << std::endl;
					send_to_client(mClient[currentFd]);
				}
				// TODO: CGI 종료 프로세스 확인(EVFILT_PROC, NOTE_EXIT in fflags)
				
			}
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
	: mServerSocket(), mClient(), kq(NULL)
{
	configType servers = g_config.getServers();
	
	for (configType::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<std::string> temp = Util::split(it->first, ':');

		Socket socket = Socket(temp[0], temp[1]); // can occur an exception
		if (fcntl(socket.get_fd(), F_SETFL, O_NONBLOCK) == -1)
			throw EventInitException("fcntl() error");
		// TODO: keepalive time 조정
		int optVal = 1;
		if (setsockopt(socket.get_fd(), SOL_SOCKET, SO_KEEPALIVE, (void*)&optVal, sizeof(int)) == -1)
			throw EventInitException("setsockopt() error");
		//
		mServerSocket[socket.get_fd()] = socket;

		logger.add_server(socket.get_fd(), it->first);
	}

	kq = new KQueue(mServerSocket);
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
	for (std::map<int, Client*>::iterator it = mClient.begin(); it != mClient.end(); ++it)
		delete it->second;
	delete kq;
}

/**
 * @brief 클라이언트의 연결을 받아 등록시키는 함수
 * 
 * @param serverFd(int) 클라이언트가 연결을 시도한 서버의 fd
 * 
 * @return None
*/
void Event::accept_connection(int serverFd)
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);

	if (clientFd == -1)
		return;
	
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		// TODO: response 500 Internal Server Error 
		return;
	}

	register_client(clientFd, clientAddr, serverFd);
	logger.connection_logging(mClient[clientFd], LOG_GREEN);
}

/**
 * @brief 클라이언트를 등록시키는 함수
 * 
 * @param clientFd(int) 클라이언트의 fd
 * @param clientAddr(struct sockaddr_in) 클라이언트의 주소 정보
 * @param serverFd(int) 클라이언트와 통신하는 서버의 fd
 * 
 * @return None
*/
void Event::register_client(int clientFd, struct sockaddr_in clientAddr, int serverFd)
{
	Client* client = new Client(clientFd, clientAddr, serverFd, \
						mServerSocket[serverFd].get_ip(), \
						mServerSocket[serverFd].get_port());

	kq->add_client_io_event(clientFd);

	mClient[clientFd] = client;
}

/**
 * @brief 클라이언트와의 연결을 끊는 함수
 * 
 * @param client(Client*) 연결을 끊을 클라이언트
 * 
 * @return None
*/
void Event::disconnection(Client* client)
{	
	logger.disconnection_logging(client, LOG_YELLOW);
	int fd = client->get_fd();

	delete mClient[fd];
	mClient.erase(fd);
	close(fd);
}


/**
 * @brief read event 발생 시 클라이언트의 메시지를 수신하는 함수
 * 
 * @param client(Client*) read event를 발생시킨 client
 * 
 * @return None
*/
// TODO : EOF 받을 시 DLE(Data Link Escape) 처리
void Event::recv_from_client(Client* client)
{
	char buf[1024];
	int recv_len = recv(client->get_fd(), buf, 1024, 0);
	buf[recv_len] = 0;
	std::cout << "recv message : " << buf << std::endl;
	kq->enable_write_event(client->get_fd());

	// int pid = fork();
	// if (pid) {
	// 	std::cout << "parent process. child pid : " << pid << std::endl;
	// 	kq->add_proc_event(pid);
	// } else {
	// 	std::cout << "child exit" << std::endl;
	// 	exit(33);
	// }
	// Request req = client->get_request();
	// int clientFd = client->get_fd();

	// int state = req.set_request(clientFd);

	// // if (state <= 0)
	// // {
	// // 	update_event(clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	// // 	update_event(clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	// // }
	// // else
	// // 	update_event(clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
}

/**
 * @brief write event 발생 시 클라이언트에게 메시지를 전송하는 함수
 * 
 * @param client(Client*) write event를 발생시킨 client
 * 
 * @return None
*/
void Event::send_to_client(Client* client)
{
	int clientFd = client->get_fd();

	// TODO : request와 response 구현 이후 작성
	int send_len = send(clientFd, "success send_to_client", strlen("success send_to_client"), 0);
	std::cout << "send_len : " << send_len << std::endl;
	kq->enable_read_event(clientFd);
	// end

	// TODO : chunked message 송신 시 수정이 필요할 수도 있음
	// update_event(clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	// update_event(clientFd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
}

// private
Event& Event::operator=(const Event& event)
{
	return *this;
}

#endif