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
# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../http.hpp"
# include "Logger.hpp"
# include "../exception/Exception.hpp"


class Event
{
public:
	typedef std::map<std::string, Server>	configType;

public:
	void event_loop();
	
	Event(const Config& config);
	~Event();

private:
	void init_server_socket(const Config& config);
	void init_kqueue();
	void update_event(uintptr_t ident, int16_t filter, \
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);
	void accept_connection(int serverFd);
	void register_client(int clientFd, struct sockaddr_in clientAddr, int serverFd);
	void disconnection(Client* client);
	void send_to_client(Client* client);
	void recv_from_client(Client* client);

	Event();
	Event& operator=(const Event& event);

private:
	std::map<int, Socket> mServerSocket;
	std::map<int, Client*> mClient;
	std::vector<struct kevent> changeList;
	int kq;
	Logger logger;
	static const int MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
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
	struct kevent* currentEvent = NULL;
	struct kevent eventList[MAX_EVENT];

	while (true)
	{
		nEvent = kevent(kq, &changeList[0], changeList.size(), eventList, MAX_EVENT, NULL);

		if (nEvent == -1)
			throw; // FIXME:

		changeList.clear();
		for (int i = 0; i < nEvent; ++i)
		{
			currentEvent = &eventList[i];
			currentFd = currentEvent->ident;

			if (currentEvent->flags & EV_ERROR)
			{
				if (mServerSocket.count(currentFd)) // server error
					throw; // FIXME: server error
				else
					disconnection(mClient[currentFd]); // client error
			}
			else if (mServerSocket.count(currentFd)) // server trigger
				accept_connection(currentFd);
			else // client trigger
			{
				if (currentEvent->flags & EV_EOF)
					disconnection(mClient[currentFd]);
				else if (currentEvent->filter == EVFILT_READ)
					recv_from_client(mClient[currentFd]);
				else if (currentEvent->filter == EVFILT_WRITE)
					send_to_client(mClient[currentFd]);
			}
		}
	}
}

/**
 * @brief config 설정에 따라 서버의 소켓과 multiplexing을 위한 kqueue를 초기화하는 생성자
 * 
 * @param config(Config) 설정 파일의 내용
 * 
 * @return None
*/
Event::Event(const Config& config)
{
	init_server_socket(config); // throw exception
	init_kqueue(); // throw exception
}

/**
 * @brief 동적 할당받은 Client 객체를 지워주는 소멸자
 * 
 * @param None
 * 
 * @return None
*/
Event::~Event()
{
	for (std::map<int, Client*>::iterator it = mClient.begin(); it != mClient.end(); ++it)
		delete it->second;
}

/**
 * @brief config 설정에 따라 server 소켓을 초기화하는 함수
 * 
 * @param config(Config) 설정 파일의 내용
 * 
 * @return None
*/
void Event::init_server_socket(const Config& config)
{
	configType servers = config.getServers();
	
	for (configType::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<std::string> temp = Util::split(it->first, ':');

		Socket socket = Socket(temp[0], temp[1]); // can occur an exception
		mServerSocket[socket.get_fd()] = socket;

		logger.add_server(socket.get_fd(), it->first);
	}
}

/**
 * @brief multiplexing을 위해 kqueue를 초기화하는 함수
 * 
 * @param None
 * 
 * @return None
*/
void Event::init_kqueue()
{
	kq = kqueue();
	if (kq == -1)
		throw EventInitException("kqueue()");
	
	for (std::map<int, Socket>::iterator it = mServerSocket.begin(); it != mServerSocket.end(); ++it)
		update_event(it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

/**
 * @brief kqueue에 event를 등록하는 함수
 * 
 * @param ident(uintptr_t) event의 식별자(주로 fd)
 * @param filter(int16_t) event를 선처리할 때 사용되는 filter
 * 	- EVFILT_READ : 읽을 data가 있을 때마다 반환
 *  - EVFILT_WRITE : 쓸 data가 있을 때마다 반환
 * @param flags(uint16_t) event에 수행할 작업
 *  - EV_ADD : kqueue에 event를 추가
 *  - EV_ENABLE : kevent() 호출 시 event 반환을 허용
 *  - EV_DISABLE : 이벤트를 비활성화하여 kevent() 호출 시 event 반환이 안됨
 * 					필터 자체는 비활성화되지 않음
 *  - EV_ERROR : 각종 error
 * @param fflags(uint32_t) filter 별 flag
 * @param data(intptr_t) filter 별 data 값
 * @param udata(void*) 명확하지 않은 user data
 * 
 * @return None
*/
void Event::update_event(uintptr_t ident, int16_t filter, uint16_t flags, \
						uint32_t fflags, intptr_t data, void* udata)
{
	struct kevent kev;
	EV_SET(&kev, ident, filter, flags, fflags, data, udata);

	changeList.push_back(kev);
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
	{
		logger.connection_failed_logging(LOG_RED);
		return;
	}
	
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		// TODO: send connection fail response 
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

	update_event(clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	update_event(clientFd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);

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
	Request req = client->get_request();
	int clientFd = client->get_fd();

	int state = req.set_request(clientFd);

	if (state <= 0)
	{
		update_event(clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
		update_event(clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	}
	else
		update_event(clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
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
	send(clientFd, "success send_to_client", strlen("success send_to_client"), 0);
	// end

	// TODO : chunked message 송신 시 수정이 필요할 수도 있음
	update_event(clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	update_event(clientFd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
}

// private
Event::Event()
{}

// private
Event& Event::operator=(const Event& event)
{
	return *this;
}

#endif