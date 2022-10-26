/**
 * TODO: change log format: $remote_addr - [$time_local] $request $status $body_bytes_sent
 */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <ctime>
# include <iostream>
# include <map>

# include "../parse/Config.hpp"
# include "Client.hpp"

static const std::string LOGCOLOR[] = {
	"\033[91m",
	"\033[92m",
	"\033[33m",
	"\033[94m",
	"\033[0m"
};

enum eLogColor
{
	LOG_RED = 0,
	LOG_GREEN,
	LOG_YELLOW,
	LOG_BLUE,
	RESET
};

class Logger
{
public:
	typedef std::map<int, std::string>	LoggerMap;

public:
	void add_server(int fd, const std::string& ip_port);
	void info();
	void connection_logging(const Client* client, enum eLogColor color);
	void connection_failed_logging(enum eLogColor color);
	void disconnection_logging(const Client* client, enum eLogColor color);

	Logger();
	~Logger();

private:
	void prefix_logMessage(const Client* client, enum eLogColor color);
	void suffix_logMessage();
	const std::string print_ip_port(const std::string& ipPort);
	const std::string print_date();

	Logger(const Logger& logger);
	Logger& operator=(const Logger& logger);

private:
	time_t now;
	LoggerMap mServer;
};

// Logger implementation

/**
 * @brief Logger가 관리하는 서버를 등록하는 함수
 * 
 * @param fd(int) 서버의 fd
 * @param ip_port(string) 서버의 ip와 port(ex. "0.0.0.0:8080")
 * 
 * @return None
*/
void Logger::add_server(int fd, const std::string& ip_port)
{
	mServer[fd] = ip_port;
}

/**
 * @brief 관리하는 서버들의 ip와 port를 출력하며 서비스의 시작을 출력하는 함수
 * 
 * @param None
 * 
 * @return None
*/
void Logger::info()
{
	now = time(0);
	char* dt = ctime(&now);

	if (dt)
		std::cout << dt;

	std::cout << "Starting Webserv version 1.0" << std::endl;
	for (LoggerMap::iterator it = mServer.begin(); it != mServer.end(); ++it)
		std::cout << "at http://" << it->second << "/" << std::endl;
	std::cout << "Quit the server with CTRL-BREAK." << std::endl;
}

/**
 * @brief 클라이언트가 connection 성공시 메시지를 출력하는 함수
 * 
 * @param client(Client*) connection을 성공한 클라이언트의 정보
 * @param color(enum eLogColor) 출력할 메시지의 색
 * 
 * @return None
*/
void Logger::connection_logging(const Client* client, enum eLogColor color)
{
	prefix_logMessage(client, color);
	std::cout << "\tConnection with client(" << client->get_ip() << ")";
	suffix_logMessage();
}

/**
 * @brief 클라이언트가 disconnection 성공시 메시지를 출력하는 함수
 * 
 * @param client(Client*) disconnection을 성공한 클라이언트의 정보
 * @param color(enum eLogColor) 출력할 메시지의 색
 * 
 * @return None
*/
void Logger::disconnection_logging(const Client* client, enum eLogColor color)
{
	prefix_logMessage(client, color);
	std::cout << "\tDisconnection with client(" << client->get_ip() << ")";
	suffix_logMessage();
}

void Logger::connection_failed_logging(enum eLogColor color)
{
	std::cout << LOGCOLOR[color] << print_date() \
			<< "\tConnection failed with client";
	suffix_logMessage();
}

Logger::~Logger()
{}

/**
 * @brief 출력 메시지의 색을 설정하여 현재 시간 정보와 해당 서버의 ip와 port를 출력하는 함수(모든 logging함수의 접두사)
 * 
 * @param client(Client*) 클라이언트의 정보
 * @param color(enum eLogColor) 출력할 메시지의 색
 * 
 * @return None
*/
void Logger::prefix_logMessage(const Client* client, enum eLogColor color)
{
	std::cout << LOGCOLOR[color] << print_date() \
			<< print_ip_port(mServer[client->get_server_fd()]);
}

/**
 * @brief 출력 메시지의 색을 초기화시키는 함수
 * 
 * @param None
 * @return None
*/
void Logger::suffix_logMessage()
{
	std::cout << LOGCOLOR[RESET] << std::endl;
}

Logger::Logger()
{}

/**
 * @brief ipPort 문자열에 대괄호를 감싸준 문자열을 반환하는 함수
 * 
 * @param ipPort(string) ip와 Port가 결합한 문자열(ex. "0.0.0.0:8080")
 * 
 * @return 대괄호로 묶은 ipPort 문자열(string)(ex. [0.0.0.0:8080])
*/
const std::string Logger::print_ip_port(const std::string& ipPort)
{
	std::string s = "";

	s += "[" + ipPort + "]";
	return s;
}

/**
 * @brief 현재 시간 정보를 가진 문자열을 반환하는 함수(format : 요일 월 일 시간:분:초 연도)
 * 
 * @param None
 * 
 * @return 현재 시간 정보를 가진 문자열(string)
*/
const std::string Logger::print_date()
{
	now = time(0);
	char* dt = ctime(&now);
	std::string s = "";

	s += "[";
	s += dt;
	s[s.size() - 1] = ']'; // replace to ] from \n

	return s; 
}

// private
Logger::Logger(const Logger& logger)
{}

// private
Logger& Logger::operator=(const Logger& logger)
{
	return *this;
}


#endif