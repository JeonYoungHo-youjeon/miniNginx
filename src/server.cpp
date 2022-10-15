#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <unordered_map>
#include <vector>

#include <iostream>

void change_events(std::vector<struct kevent>& change_list, \
		uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, \
		intptr_t data, void* udata) {
	struct kevent tmp_event;

	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(tmp_event);
}

int main(int argc, char** argv) {
	//	Parsing
	
	//	Created socket
	
	/*
	 *	if parsed - port : 4242
	 */
	std::string server_name = "localhost";
	std::string port = "4242";
	struct sockaddr_in server_addr;
	int server_fd;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;					//	Protocol
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//	Addr
	server_addr.sin_port = htons(std::stoi(port));		//	Port

	/*
	 *	endif parsed
	 */
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Socket Error" << std::endl;
		return -1;
	}

	//	Binding
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "Bind Error" << std::endl;
		return -1;
	}
	//	Listening
	if (listen(server_fd, 20) == -1) {	//	Socket_discriptor, Accept Waiting available
		std::cerr << "Listen Error" << std::endl;
		return -1;
	}

	//	Set Socket to Non-Blocking
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	/*
	 *	***	Created * KQUEUE ***
	 */
	int kq;
	if ((kq = kqueue()) == -1) {
		std::cerr << "Kqueue Error" << std::endl;
		return -1;
	}

	std::unordered_map<int, std::string> clients;
	std::vector<struct kevent> change_list;
	const size_t size = 20;
	struct kevent event_list[size];

	//	Add Event
	change_events(change_list, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "Server [" << server_fd << "] Starts" << std::endl;

	//	Loop
	int new_events;
	struct kevent* curr_event;
	while (true)
	{
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, size, NULL);
		if (new_events == -1) {
			std::cerr << "Kevent Error" << std::endl;
			return -1;
		}
		change_list.clear();	//	Changed Clear & Ready For New Change

		for (int i = 0; i < new_events; ++i) {
			curr_event = &event_list[i];

			if (curr_event->flags & EV_ERROR) {
				if (curr_event->ident == server_fd) {
					std::cerr << "Server Error" << std::endl;
					return -1;
				}
				else {
					std::cerr << "Client [" << curr_event->ident << "] occurs Error" << std::endl;
					close(curr_event->ident);
					clients.erase(curr_event->ident);
				}
			} else if (curr_event->filter == EVFILT_READ) {
				if (curr_event->ident == server_fd) {
					//	Accept
					int client_fd;
					if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
						std::cerr << "Client Error" << std::endl;
						return -1;
					}
					std::cout << "New Client[" << client_fd << "] is Connected" << std::endl;
					fcntl(client_fd, F_SETFL, O_NONBLOCK);

					//	Add Event Client
					change_events(change_list, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(change_list, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					clients[client_fd] = "";
				} else if (clients.find(curr_event->ident) != clients.end()) {
					//	Read Data
					char buf[4096];
					//int read_size = read(curr_event->ident, buf, sizeof(buf));
					int read_size = recv(curr_event->ident, buf, sizeof(buf), 0);

					if (read_size < 0) {
						std::cerr << "Read Error" << std::endl;
						close(curr_event->ident);
						clients.erase(curr_event->ident);
					} else if (read_size) {
						buf[read_size] = 0;
						clients[curr_event->ident] += buf;
						std::cout << "Client [" << curr_event->ident << "] : " << clients[curr_event->ident] << std::endl;
					}
				}
			} else if (curr_event->filter == EVFILT_WRITE) {
				//	Send Data
				if (send(curr_event->ident, clients[curr_event->ident].c_str(), clients[curr_event->ident].size(), 0) < 0) {
					close(curr_event->ident);
					clients.erase(curr_event->ident);
					std::cerr << "Send Error" << std::endl;
				} else clients[curr_event->ident].clear();
			}
		}
	}
	return 0;
}
