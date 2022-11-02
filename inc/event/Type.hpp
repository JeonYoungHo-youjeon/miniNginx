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
# include "../define.hpp"

typedef int 							FD;
typedef struct kevent					KEvent;
typedef std::vector<KEvent>		 		ChangeList;
typedef struct sockaddr_in				SockAddr;
typedef std::string						String;
typedef std::map<std::string, Server>	ConfigType;
typedef time_t							Time;
typedef int								Type;

#endif // TYPE_HPP