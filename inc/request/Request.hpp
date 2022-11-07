#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sys/socket.h>

# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../response/Cgi.hpp"
// # include "../Body.hpp"

using std::string;
using std::cout;
using std::endl;

extern Config g_conf;

struct RequestStartLine
{
	string method;			// GET, POST, DELETE 등의 메소드	*
	string url;				// 요청 URL (./ , /www/...)		*
	string protocol;		// HTTP 버전 (HTTP/1.1)

	void out()
	{
		cout << "[    Start Line    ]" << endl;
		cout << "[" << method << "] " << "[" << url << "] " << "[" << protocol << "]" << endl;
	}
};

struct Request
{
	RequestStartLine			StartLine;
	std::map<string, string>	Header;
	std::string 				Body;

	string						configName;
	string						locationName;
	string 						fileName;
	string 						ext;
	std::stringstream			buffer;
	int 						statement;
	int 						progress;
	std::string					tmp;

	string virtualPath;

	int contentLength;
	bool chunkFlag;
	int statusCode;
	int clientFd;

	std::vector<string>	params;

	/**
	* @brief : 생성자 초기화, set()을 통한 초기화 등 임의로 다양하게 구현
	*/
	Request() : statusCode(200) {};
	Request(int fd, const string& configName)
	: statusCode(200), configName(configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		statement = READ_REQUEST;
		progress = START_LINE;
	};

	Request& operator=(const Request& req)
	{
		return *this;
	}

	int set(int fd, const string& configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		statement = READ_REQUEST;
		progress = START_LINE;

		return READ_REQUEST;
	}

#include <iostream>
	int read()
	{
		char rcvData[BUFFER_SIZE];
		int byte = recv(clientFd, &rcvData[0], BUFFER_SIZE, 0);

		if (byte < 0)
		{
			strerror(errno);
			throw statusCode = 400;	//	catch (int statusCode);
		}
		tmp += rcvData;
		//buffer << rcvData;
		//std::cout << buffer.str() << std::endl;
		for (int i = 0; i < tmp.size(); ++i) {
			std::cout << tmp[i] << " : " << (int)tmp[i] << std::endl;
		}
		if (buffer.str().back() != '\n') {
		std::cout << "back" << std::endl;
			return statement = READ_REQUEST;
		}

		return statement = parse();
	}

	int parse()
	{
		std::cout << "start pares" << std::endl;
		string tmpBuf;

		//	STARTLINE
		if (progress == START_LINE)
		{
			std::getline(buffer, tmpBuf, '\n');
			std::stringstream tmpSs(Util::remove_crlf(tmpBuf));

			tmpSs >> StartLine.method >> StartLine.url >> StartLine.protocol;
			if (!tmpSs || !tmpSs.str().empty())
				throw "Format Error";
			progress = HEADER;
		}

		//	HEADER
		while (progress == HEADER)
		{
			std::getline(buffer, tmpBuf);

			Util::strip(tmpBuf, '\r');
			if (tmpBuf.empty())
				progress = HEADER_SET;
			else
				makeHeader(tmpBuf);
		}

		//	HEADER SETTING
		if (progress == HEADER_SET)
		{
			virtualPath = Util::split(StartLine.url, '?')[0];
			params = Util::split(Util::split(StartLine.url, '?')[1], '&');
			locationName = findLocation(virtualPath);
			fileName = virtualPath.erase(0, locationName.size());
			ext = findExtension(virtualPath);


			if (Header.find("Transfer-Encoding") != Header.end()
				^ Header.find("Content-Length") != Header.end())
				throw "Request 133L Chunked && Content-Length";

			if (Header.find("Content-Length") != Header.end())
				contentLength = Util::stoi(Header["Content-Length"]);

			if (Header.find("Transfer-Encoding") != Header.end())
				contentLength = chunkFlag = true;

			if (StartLine.method != "POST" && (progress = PROG_DONE))
				return DONE_REQUEST;
			progress = BODY;
		}

		//	BODY
		int		bodyMax = 0;
		if (g_conf[configName][locationName].is_exist("client_max_body_size"))
			bodyMax = Util::stoi(g_conf[configName][locationName]["client_max_body_size"][0]);

		while (progress == BODY && contentLength > 0)
		{
			std::getline(buffer, tmpBuf);
			Util::remove_crlf(tmpBuf);

			//	버퍼가 비었을 때 강제로 스코프 탈출시켜 if로 예외처리 되게 함
			if (tmpBuf.empty())
				break ;

			Util::join(Body, tmpBuf, '\n');
			contentLength -= tmpBuf.size();

			if (chunkFlag)
			{
				std::getline(buffer, tmpBuf);
				contentLength = Util::to_hex(tmpBuf);
			}

			if (bodyMax && Body.size() > bodyMax)
				throw statusCode = 413;
			std::cout << "in" << std::endl;
		}
		std::cout << "out" << std::endl;
		if (contentLength)
			return READ_REQUEST;

		return DONE_REQUEST;
	}

	void makeBody()
	{
		if (!contentLength)
			return ;
	}

	void makeHeader(const std::string& buf)
	{
		std::pair<std::string, std::string> kv = Util::divider(buf, ':');

		//	TODO : 정규화 필요 여부 확인
		if (kv.first.empty() || kv.second.empty())
			throw statusCode = 400;	//	bad Request
		if (Header.find(kv.first) != Header.end())
			throw statusCode = 400;

		Header[kv.first] = kv.second;
	}

	//	TODO : 맘에 안드는 함수
	std::string findLocation(const std::string& path)
	{
		std::string ret;
		std::string tmp;
		vector<std::string> pathTree = Util::split(path, '/');

		for (std::vector<std::string>::iterator it = pathTree.begin(); it != pathTree.end(); ++it)
			if (g_conf[configName].is_exist(tmp = Util::join(tmp, *it, '/')))
				ret = tmp;
		return ret;
	};

	std::string findExtension(const std::string& url)
	{
		std::string::size_type pos = url.rfind('.');
		if (pos == std::string::npos)
			return "";
		return std::string(url.begin() + pos, url.end());
	}

	/**
	*  현재 리퀘스트 구조체의 내용 전체를 출력. 빈 변수는 출력하지 않음
	*
	*/
	void print_request()
	{
		StartLine.out();
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			cout << it->first << " : " << it->second << endl;
	}
};

#endif //REQUEST_H
