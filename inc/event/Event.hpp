#ifndef EVENT_HPP
# define EVENT_HPP


# include <vector>
# include <map>
# include <string>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>

# include "Socket.hpp"
# include "Client.hpp"
# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../http.hpp"
# include "Logger.hpp"


class Event
{
public:
	typedef std::map<std::string, Server>	configType;

public:
	void event_loop();
	
	Event(const Config& config);
	~Event();

protected:

private:
	bool init_kqueue();
	void update_event(uintptr_t ident, int16_t filter, \
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);
	void accept_connection(int serverFd);
	void send_to_client(Client* client);
	void recv_from_client(Client* client);
	void disconnection(Client* client);

public:

protected:

private:
	std::map<int, Socket> mServerSocket;
	std::map<int, Client*> mClient;
	int kq;
	static const int MAX_EVENT = 1024;
	Logger logger;
};

// Event implementation

void Event::update_event(uintptr_t ident, int16_t filter, \
				uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
{
	struct kevent kev;
	EV_SET(&kev, ident, filter, flags, fflags, data, udata);
	kevent(kq, &kev, 1, NULL, 0, NULL);
}

void Event::event_loop()
{
	if (init_kqueue() == false)
		throw; // FIXME: kqueue error

	logger.info();

	int nEvent, currentFd;
	struct kevent* currentEvent = NULL;
	struct kevent eventList[MAX_EVENT];

	while (true)
	{
		nEvent = kevent(kq, NULL, 0, eventList, MAX_EVENT, NULL);

		if (nEvent == -1)
			throw; // FIXME

		for (int i = 0; i < nEvent; ++i)
		{
			currentEvent = &eventList[i];
			currentFd = currentEvent->ident;

			if (currentEvent->flags & EV_ERROR)
			{
				if (mServerSocket.count(currentFd))
					throw; // FIXME: server error
				else
					disconnection(mClient[currentFd]);
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

void Event::disconnection(Client* client)
{
	int fd = client->get_fd();

	update_event(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	update_event(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	close(fd);
	mClient.erase(fd);
	delete(client);
	logger.logging(client->get_server_fd(), client->get_ip(), \
					GREEN, DISCONNECTION);

}

void Event::send_to_client(Client* client)
{
	int clientFd = client->get_fd();

	send(clientFd, "success send_to_client", strlen("success send_to_client"),0);

	update_event(clientFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
	update_event(clientFd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
}


void Event::recv_from_client(Client* client)
{
	char buf[1024] = {};
	int recv_len = 0;
	int clientFd = client->get_fd();


	recv_len = recv(clientFd, buf, 1024, 0);

	std::string str = buf;

	if (recv_len == -1)
		throw; // FIXME: recv error
	else if (recv_len == 0)
		disconnection(client);

	std::cout << "success recv_from_client : " << buf << std::endl;
	update_event(clientFd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	update_event(clientFd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
}

void Event::accept_connection(int serverFd)
{
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);

	if (clientFd == -1)
		return;
	
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	Client* client = new Client(clientFd, serverFd, clientAddr, \
						mServerSocket[serverFd].get_ip(), \
						mServerSocket[serverFd].get_port());

	update_event(clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	update_event(clientFd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);

	mClient[clientFd] = client;

	logger.logging(serverFd, client->get_ip(), GREEN, CONNECTION);
}

Event::Event(const Config& config)
{
	configType servers = config.getServers();
	for (configType::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<std::string> temp = Util::split(it->first, ':');

		try
		{
			Socket socket = Socket(temp[0], temp[1]);
			mServerSocket[socket.get_fd()] = socket;
			logger.add_server(socket.get_fd(), it->first);
		}
		catch(const std::exception& e) // FIXME:
		{
			throw;
		}
	}
}

bool Event::init_kqueue()
{
	kq = kqueue();
	if (kq == -1)
		return false;
	
	for (std::map<int, Socket>::iterator it = mServerSocket.begin(); it != mServerSocket.end(); ++it)
		update_event(it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	return true;
}

Event::~Event()
{
	for (std::map<int, Client*>::iterator it = mClient.begin(); it != mClient.end(); ++it)
		delete it->second;
}


#endif