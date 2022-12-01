#ifndef KQUEUE_HPP
# define KQUEUE_HPP


# include "Type.hpp"
# include "ClientSocket.hpp"

class KQueue {
public:

	const KEvent* get_eventList() const;
	const ChangeList& get_changeList() const;
	int wait_event();
	void set_server_event(const Socket* socket, FD fd);
	void set_client_event(const Socket* socket, FD fd);
	void set_next_event(ClientSocket* socket, State state);
	void on_read_event(const Socket* socket, FD fd);
	void on_write_event(const Socket* socket, FD fd);
	void off_read_event(const Socket* socket, FD fd);
	void off_write_event(const Socket* socket, FD fd);
	// void delete_event(const Socket* socket, FD fd)
	// {
	// 	update_event(fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)socket);
	// 	update_event(fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)socket);
	// }

	KQueue();
	~KQueue();

	void update_event(uintptr_t ident, int16_t filter, \
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);
private:
	void init_kqueue();
	void add_timeout(const Socket* socket);
	void add_read_event(const Socket* socket, FD fd);
	void add_write_event(const Socket* socket, FD fd);
	void add_proc_event(const Socket* socket, pid_t pid);
	void enable_read_event(const Socket* socket, FD fd);
	void enable_write_event(const Socket* socket, FD fd);
	void disable_read_event(const Socket* socket, FD fd);
	void disable_write_event(const Socket* socket, FD fd);

	KQueue(const KQueue& other);
	KQueue operator=(const KQueue& rhs);

private:
	FD kq;
	ChangeList changeList;
	KEvent eventList[MAX_EVENT];
};

// KQueue implementation

const KEvent* KQueue::get_eventList() const
{
	return eventList;
}

const ChangeList& KQueue::get_changeList() const
{
	return changeList;
}

int KQueue::wait_event()
{
	// TODO:
	struct timespec tmout;
	tmout.tv_nsec = 0;
	tmout.tv_sec = 1;
	int nEvent = kevent(kq, &changeList[0], changeList.size(), eventList, MAX_EVENT, &tmout);
	
	if (nEvent == -1)
		throw std::runtime_error("kevent");
	changeList.clear();
	return nEvent;
}

void KQueue::set_server_event(const Socket* socket, FD fd)
{
	add_read_event(socket, fd);
}

void KQueue::set_client_event(const Socket* socket, FD fd)
{
	add_read_event(socket, fd);
	add_write_event(socket, fd);
	add_timeout(socket);
}

void KQueue::on_read_event(const Socket* socket, FD fd)
{
	enable_read_event(socket, fd);
	disable_write_event(socket, fd);
	add_timeout(socket);
}

void KQueue::on_write_event(const Socket* socket, FD fd)
{
	enable_write_event(socket, fd);
	add_timeout(socket);
}

void KQueue::off_read_event(const Socket* socket, FD fd)
{
	disable_read_event(socket, fd);
	disable_write_event(socket, fd);
}

void KQueue::off_write_event(const Socket* socket, FD fd)
{
	disable_write_event(socket, fd);
}

void KQueue::add_read_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, (void*)socket);
}

void KQueue::add_write_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void*)socket);
}

void KQueue::enable_read_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_READ, EV_ENABLE | EV_CLEAR, 0, 0, (void*)socket);
}

void KQueue::enable_write_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, (void*)socket);
}

void KQueue::disable_read_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_READ, EV_DISABLE | EV_CLEAR, 0, 0, (void*)socket);
}

void KQueue::disable_write_event(const Socket* socket, FD fd)
{
	update_event(fd, EVFILT_WRITE, EV_DISABLE | EV_CLEAR, 0, 0, (void*)socket);
}

void KQueue::add_proc_event(const Socket* socket, pid_t pid)
{
	update_event(pid, EVFILT_PROC, EV_ADD, NOTE_EXITSTATUS, 0, (void*)socket);
}

void KQueue::set_next_event(ClientSocket* socket, State state)
{
	const Response* res = socket->get_response();

	switch (state)
	{
	case READ_REQUEST:
		PRINT_LOG("NEXT_READ_REQUEST");
		on_read_event(socket, socket->get_fd());
		break;
	case READ_RESPONSE:
		PRINT_LOG("NEXT_READ_RESPONSE");
		if (!socket->get_PID() && res->contentResult->getPid())
		{
			socket->set_PID(res->contentResult->getPid());
			add_proc_event(socket, socket->get_PID());
		}
		if (!socket->get_readFD() && res->contentResult->outFd \
			&& !socket->get_readFD() && res->contentResult->inFd)
		{
			socket->set_readFD(res->contentResult->outFd);
			socket->set_writeFD(res->contentResult->inFd);
			set_client_event(socket, socket->get_readFD());
			set_client_event(socket, socket->get_writeFD());
		}

		on_read_event(socket, socket->get_readFD());
		off_write_event(socket, socket->get_writeFD());
		break;
	case WRITE_RESPONSE:
		PRINT_LOG("NEXT_WRITE_RESPONSE");
		if (!socket->get_PID() && res->contentResult->getPid())
		{
			socket->set_PID(res->contentResult->getPid());
			add_proc_event(socket, socket->get_PID());
		}
		if (!socket->get_readFD() && res->contentResult->outFd \
			&& !socket->get_readFD() && res->contentResult->inFd)
		{
			socket->set_readFD(res->contentResult->outFd);
			socket->set_writeFD(res->contentResult->inFd);
			set_client_event(socket, socket->get_readFD());
			set_client_event(socket, socket->get_writeFD());
		}

		on_write_event(socket, socket->get_writeFD());
		break;
	case SEND_RESPONSE:
		PRINT_LOG("NEXT_SEND_RESPONSE");
		on_write_event(socket, socket->get_fd());
		break;
	}
}


KQueue::KQueue()
{
	init_kqueue();
}

KQueue::~KQueue()
{
	close(kq);
}

void KQueue::init_kqueue()
{
	kq = kqueue();

	if (kq == -1)
		throw std::runtime_error("kqueue");

}

void KQueue::update_event(uintptr_t ident, int16_t filter, uint16_t flags, \
						uint32_t fflags, intptr_t data, void* udata)
{
	KEvent kev;
	EV_SET(&kev, ident, filter, flags, fflags, data, udata);

	changeList.push_back(kev);
}

void KQueue::add_timeout(const Socket* socket)
{
	update_event(socket->get_fd(), EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT, (void*)socket);
}

// private
KQueue::KQueue(const KQueue& other)
{}

// private
KQueue KQueue::operator=(const KQueue& rhs)
{
	return *this;
}

#endif