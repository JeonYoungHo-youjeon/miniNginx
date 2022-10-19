#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <ctime>
# include <iostream>
# include <map>

# include "../parse/Config.hpp"

static const std::string COLOR[] = {
	"\033[91m",
	"\033[92m",
	"\033[33m",
	"\033[94m",
	"\033[0m"
};

enum eColor
{
	RED = 0,
	GREEN,
	YELLOW,
	BLUE,
	END
};

enum eEvent
{
	CONNECTION = 0,
	DISCONNECTION,
	REQUEST
};

class Logger
{
public:
	typedef std::map<int, std::string>	loggerType;

public:
	void add_server(int fd, const std::string& ip_port)
	{
		mServer[fd] = ip_port;
	}

	void info()
	{
		now = time(0);
		char* dt = ctime(&now);

		if (dt)
			std::cout << dt;

		std::cout << "Starting Webserv version 1.0" << std::endl;
		for (loggerType::iterator it = mServer.begin(); it != mServer.end(); ++it)
			std::cout << "at http://" << it->second << "/" << std::endl;
		std::cout << "Quit the server with CTRL-BREAK." << std::endl;
	}

	void logging(int serverFd, const std::string& clientIp, \
				enum eColor color, enum eEvent event)
	{

		std::cout << COLOR[color] << print_date() \
				<< print_ip_port(mServer[serverFd]);

		switch (event)
		{
		case CONNECTION:
			std::cout << "\tConnection with client(" << clientIp << ")";
			break;
		case DISCONNECTION:
			std::cout << "\tDisconnection with client(" << clientIp << ")";
			break;
		case REQUEST:
			break;
		default:
			break;
		}

		std::cout << COLOR[END] << std::endl;
	}

	Logger();
protected:

private:
	const std::string print_ip_port(const std::string& ipPort)
	{
		std::string s = "";

		s += "[";
		s += "[" + ipPort + "]";
		return s;
	}

	const std::string print_date()
	{
		now = time(0);
		char* dt = ctime(&now);
		std::string s = "";

		s += "[";
		s += dt;
		s[s.size() - 1] = ']'; // replace to ] from \n

		return s; 
	}
public:

protected:

private:
	time_t now;
	loggerType mServer;
};

// Logger implementation

Logger::Logger()
{}

#endif