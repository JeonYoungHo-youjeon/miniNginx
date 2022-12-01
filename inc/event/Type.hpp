#ifndef TYPE_HPP
# define TYPE_HPP

# include <map>
# include <string>
# include <ctime>
# include <sys/event.h>
# include <netinet/in.h>

# include "../parse/Server.hpp"

void print_log(const std::string title)
{
	std::cout << "\t==========[" << title << "]==========" << std::endl;
}

// # define PRINT_LOG(title) print_log(title)
# define PRINT_LOG(title)


typedef int 							FD;
typedef struct kevent					KEvent;
typedef std::vector<KEvent>		 		ChangeList;
typedef struct sockaddr_in				SockAddr;
typedef std::string						String;
typedef std::map<std::string, Server>	ConfigType;
typedef time_t							Time;
typedef int								Type;
typedef int								State;
typedef pid_t							PID;

#endif // TYPE_HPP