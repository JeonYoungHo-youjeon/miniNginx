#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <sys/event.h>
# include <map>
# include <vector>
# include <time.h>

# include "Socket.hpp"
# include "../exception/Exception.hpp"

class KQueue {
public:
	typedef int 						FD;
	typedef std::map<int, Socket> 		ServerSocketMap;
	typedef struct kevent				KEvent;
	typedef std::vector<KEvent>		 	ChangeList;
public:
	const KEvent* get_eventList() const;
	const ChangeList& get_changeList() const;
	int wait_event();
	void add_io_event(FD fd);
	void add_client_io_event(FD fd);
	void enable_read_event(FD fd);
	void enable_write_event(FD fd);
	void add_proc_event(pid_t pid) {
		update_event(pid, EVFILT_PROC, EV_ADD, NOTE_EXITSTATUS, 0, NULL);
	}

	KQueue(const ServerSocketMap& sockets);
	~KQueue();
private:
	KQueue();
	KQueue(const KQueue& other);
	KQueue operator=(const KQueue& rhs);

	void init_kqueue();
	void update_event(uintptr_t ident, int16_t filter, \
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);
public:
private:
	FD kq;
	ChangeList changeList;
	static const int MAX_EVENT = 1024;
	KEvent eventList[MAX_EVENT];
};

// KQueue implementation

const KQueue::KEvent* KQueue::get_eventList() const
{
	return eventList;
}

const KQueue::ChangeList& KQueue::get_changeList() const
{
	return changeList;
}

int KQueue::wait_event()
{
	// TODO: timeout 설정
	int nEvent = kevent(kq, &changeList[0], changeList.size(), eventList, MAX_EVENT, NULL);
	
	if (nEvent == -1)
		return -1;

	changeList.clear();

	return nEvent;
}

void KQueue::add_io_event(FD fd)
{
	update_event(fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
	update_event(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);

	// update_event(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	// update_event(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
}

void KQueue::add_client_io_event(FD fd)
{
	add_io_event(fd);
	update_event(fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 3000, 0);
	// update_event(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	// update_event(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
}

void KQueue::enable_read_event(FD fd)
{
	update_event(fd, EVFILT_READ, EV_ENABLE | EV_CLEAR, 0, 0, NULL);
	update_event(fd, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
	update_event(fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 3000, 0);

}

void KQueue::enable_write_event(FD fd)
{
	update_event(fd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	update_event(fd, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, NULL);
}

KQueue::KQueue(const ServerSocketMap& sockets)
{
	init_kqueue();

	for (ServerSocketMap::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
		add_io_event(it->first);
}

KQueue::~KQueue()
{
	close(kq);
}

// private
KQueue::KQueue(const KQueue& other)
{}

// private
KQueue KQueue::operator=(const KQueue& rhs)
{
	return *this;
}

void KQueue::init_kqueue()
{
	kq = kqueue();
	if (kq == -1)
		throw EventInitException("kqueue() error");
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

#endif