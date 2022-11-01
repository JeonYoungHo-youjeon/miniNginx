#ifndef TYPE_HPP
# define TYPE_HPP

// Cpp header files
# include <map>
# include <vector>
# include <string>
# include <cerrno>
# include <ctime>
# include <utility>

// System header files
# include <sys/event.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>

// User defined header files
# include "../parse/Config.hpp"


typedef int 							FD;
typedef struct kevent					KEvent;
typedef std::vector<KEvent>		 		ChangeList;
typedef struct sockaddr_in				SockAddr;
typedef std::string						String;
typedef std::map<std::string, Server>	ConfigType;
typedef time_t							Time;
typedef int								Type;


enum eType {
	SERVER = 0,
	CLIENT
};

static const Time TIMEOUT = 10;
static const int MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.


#endif // TYPE_HPP