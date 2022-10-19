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

# include "Socket.hpp"
# include "Client.hpp"
# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../http.hpp"

# include <unistd.h>

class Event
{
public:
	typedef std::map<std::string, Server>	configType;

public:

	void update_event(uintptr_t ident, int16_t filter, \
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
	{
		struct kevent kev;
		EV_SET(&kev, ident, filter, flags, fflags, data, udata);
		kevent(kq, &kev, 1, NULL, 0, NULL);
	}

	void event_loop()
	{
		std::cout << "event loop start" << std::endl;
		if (init_kqueue() == false)
			throw; // FIXME: kqueue error

		int nEvent;
		struct kevent* currEvent = NULL;
		struct kevent eventList[MAX_EVENT];
		// TODO: timeout 설정
		// struct timespec timeout;

		while (true)
		{
			nEvent = kevent(kq, NULL, 0, eventList, MAX_EVENT, NULL);
			if (nEvent == -1)
				throw; // FIXME:

			for (int i = 0; i < nEvent; ++i)
			{
				currEvent = &eventList[i];

				if (currEvent->flags & EV_ERROR)
				{
					if (mSocket.count(currEvent->ident))
						throw; // FIXME: server error
					else
						throw; // FIXME: client error
				}
				else if (mSocket.count(currEvent->ident)) // server trigger
				{
					std::cout << "accept to client(server socket : " << currEvent->ident << ")" << std::endl;
					if (accept_connection(currEvent->ident) == false)
						throw;
				}
				else // client trigger
				{
					if (currEvent->filter == EVFILT_READ)
					{
						std::cout << "EVFILT_READ occur" << std::endl;
						recv_from_client(currEvent->ident);
						update_event(currEvent->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
						update_event(currEvent->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
						// update_event(currEvent->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
					}
					else if (currEvent->filter == EVFILT_WRITE)
					{
						std::cout << "EVFILT_READ occur" << std::endl;
						send_to_client(currEvent->ident);
						update_event(currEvent->ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
						update_event(currEvent->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
					}
					else if (currEvent->flags & EV_EOF)
					{
						std::cout << "disconnection" << std::endl;
						close(currEvent->ident);
						delete mClient[currEvent->ident];
					}
				}
			}
		}
	}

	void send_to_client(int client_fd)
	{
		Response res;

		res.protocol = "HTTP/1.1";	
		res.statusCode = "200";	
		res.contentLength = "12";	
		res.connection = "close";	
		res.server = "webserv";
		// res.body = req.body;
		res.print_response();
		std::cout << res.get_response().c_str() << std::endl;
		send(client_fd, res.get_response().c_str(), res.get_response().size(), 0);

	}

	void recv_from_client(int client_fd)
	{
		char buf[1024] = {};
		int recv_len = 0;


		recv_len = recv(client_fd, buf, 1024, 0);

		std::string str = buf;

		if (recv_len <= 0)
		{
			if (recv_len < 0)
				std::cerr << "client read error!" << std::endl;
			std::cout << "close input" << std::endl;
		}

		std::cout << "Client Msg = " << buf << std::endl;
		std::cout << str << std::endl;

		Request req;
		
		try
		{
			req.set_request(str);
		}
		catch (...)
		{
			std::cerr << "TEST" << endl;
		}

		req.print_request();
	}

	bool accept_connection(int serverFd)
	{
		std::cout << "accept_connection" << std::endl;
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if (clientFd == -1)
			return false;
		
		fcntl(clientFd, F_SETFL, O_NONBLOCK);

		Client* client = new Client(clientFd, clientAddr);

		update_event(clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		update_event(clientFd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
		// update_event(clientFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

		mClient[clientFd] = client;

		return true;
	}

	Event(const Config& config)
	{
		std::map<std::string, Server> servers = config.getServers();
		for (configType::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::vector<std::string> temp = Util::split(it->first, ':');
			std::cout << it->first << std::endl;
			try
			{
				Socket socket = Socket(temp[0], temp[1]);
				mSocket[socket.get_fd()] = socket;
			}
			catch(const std::exception& e) // FIXME:
			{
				throw;
			}
		}
	}
protected:

private:
	bool init_kqueue()
	{
		kq = kqueue();
		if (kq == -1)
			return false;
		
		for (std::map<int, Socket>::iterator it = mSocket.begin(); it != mSocket.end(); ++it)
			update_event(it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			
		return true;
	}

public:

protected:

private:
	std::map<int, Socket> mSocket;
	std::map<int, Client*> mClient;
	int kq;
	static const int MAX_EVENT = 1024;
};

#endif