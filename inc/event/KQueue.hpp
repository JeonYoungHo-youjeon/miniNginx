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
	int nEvent = kevent(kq, &changeList[0], changeList.size(), eventList, MAX_EVENT, NULL);

	// TODO: error 발생 시 500 Inerneal Server Error를 던져야 하는 상황 확인
	// switch (errno)
	// {
	// case EACCES:
	// 	throw EventInitException("kevent() error [errno : EACCES]");
	// case EFAULT:
	// 	throw EventInitException("kevent() error [errno : EFAULT]");
	// case EBADF:
	// 	throw EventInitException("kevent() error [errno : EBADF]");
	// case EINTR:
	// 	throw EventInitException("kevent() error [errno : EINTR]");
	// case EINVAL:
	// 	throw EventInitException("kevent() error [errno : EINVAL]");
	// case ENOENT:
	// 	throw EventInitException("kevent() error [errno : ENOENT]");
	// case ENOMEM:
	// 	throw EventInitException("kevent() error [errno : ENOMEM]");
	// case ESRCH:
	// 	throw EventInitException("kevent() error [errno : ESRCH]");
	// default:
	// 	changeList.clear();
	// 	return nEvent;
	// }
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

// void KQueue::enable_read_event(const Socket* socket, FD fd)
// {
// 	update_event(fd, EVFILT_READ, EV_ENABLE | EV_CLEAR, 0, 0, (void*)socket);
// 	update_event(fd, EVFILT_WRITE, EV_DISABLE, 0, 0, (void*)socket);
// 	add_timeout(socket);
// }

// void KQueue::enable_write_event(const Socket* socket, FD fd)
// {
// 	update_event(fd, EVFILT_READ, EV_DISABLE, 0, 0, (void*)socket);
// 	update_event(fd, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, (void*)socket);
// }

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
		std::cout << "\t==========[NEXT READ_REQUEST]==========" << std::endl;

		on_read_event(socket, socket->get_fd());
		break;
	case READ_RESPONSE:
		std::cout << "\t==========[NEXT READ_RESPONSE]==========" << std::endl;
		if (!socket->get_PID() && res->contentResult->getPid())
		{
			socket->set_PID(res->contentResult->getPid());
			add_proc_event(socket, socket->get_PID());
		}
		if (!socket->get_readFD() && res->contentResult->outFd)
		{
			socket->set_readFD(res->contentResult->outFd);
			set_client_event(socket, socket->get_readFD());
		}

		on_read_event(socket, socket->get_readFD());
		break;
	case WRITE_RESPONSE:
		std::cout << "\t==========[NEXT WRITE_RESPONSE]==========" << std::endl;
		if (!socket->get_PID() && res->contentResult->getPid())
		{
			socket->set_PID(res->contentResult->getPid());
			std::cout << "socket->get_pid() : " << socket->get_PID() << std::endl;
			add_proc_event(socket, socket->get_PID());
		}
		if (!socket->get_writeFD() && res->contentResult->inFd)
		{
			socket->set_writeFD(res->contentResult->inFd);
			// TODO: change function name
			set_client_event(socket, socket->get_writeFD());
		}

		on_write_event(socket, socket->get_writeFD());
		break;
	case SEND_RESPONSE:
		std::cout << "\t==========[NEXT SEND_RESPONSE]==========" << std::endl;
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

	switch (errno)
	{
	case ENOMEM:
		throw EventInitException("kqueue() error [errno : ENOMEM]");
	case EMFILE:
		throw EventInitException("kqueue() error [errno : EMFILE]");
	default:
		break;
	}
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