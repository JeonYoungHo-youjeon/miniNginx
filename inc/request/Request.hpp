#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sys/socket.h>

# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../response/Cgi.hpp"

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

	void set(int fd, const string& configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		statement = READ_REQUEST;
		progress = START_LINE;
	}

	int read()
	{
		char rcvData[BUFFER_SIZE] = {0};
		int byte = recv(clientFd, &rcvData[0], BUFFER_SIZE, 0);

		if (byte <= 0)
		{
			strerror(errno);
			throw statusCode = 400;	//	catch (int statusCode);
		}
		buffer << rcvData;
		if (buffer.str().find('\n') == std::string::npos)
			return statement = READ_REQUEST;
		return statement = parse();
	}

	int parse()
	{
		cout << "progress == START_LINE" << endl;
		if (progress == START_LINE)
		{
			if (parse_startline() == false)
				throw statusCode = 400;

			StartLine.out();
			progress = HEADER;
			return READ_REQUEST;
		}
		cout << "progress == HEADER" << endl;

		while (progress == HEADER)
		{
			if (is_done_buffer() == true)
				return READ_REQUEST;

			if (is_end_header() == true) {
				progress = HEADER_SET;
				break;
			}

			std::getline(buffer, tmp);
			make_header(tmp);
		}
		cout << "progress == HEADER_SET" << endl;

		if (progress == HEADER_SET)
		{
			// virtualPath : ?로 나눌 때 [0]
			// params : ?로 나눌 때 [1]
			virtualPath = Util::split(StartLine.url, '?')[0];
			params = Util::split(Util::split(StartLine.url, '?')[1], '&');
			locationName = findLocation(virtualPath);
			fileName = virtualPath.erase(0, locationName.size());
			ext = findExtension(virtualPath);

			if (Header.find("Transfer-Encoding") != Header.end()
				^ Header.find("Content-Length") != Header.end()) {
					std::cout << Header["Content-Length"] << std::endl;
					throw "Request 133L Chunked && Content-Length";
				}

			if (Header.find("Content-Length") != Header.end())
				contentLength = Util::stoi(Header["Content-Length"]);

			if (Header.find("Transfer-Encoding") != Header.end())
				contentLength = chunkFlag = true;

			if (StartLine.method != "POST" && (progress = PROG_DONE))
				return DONE_REQUEST;
			progress = BODY;
		}

		//	BODY
		// int		bodyMax = 0;
		// if (g_conf[configName][locationName].is_exist("client_max_body_size"))
		// 	bodyMax = Util::stoi(g_conf[configName][locationName]["client_max_body_size"][0]);

		// while (progress == BODY && contentLength > 0)
		// {
		// 	Util::getline(buffer, tmpBuf);
		// 	Util::remove_crlf(tmpBuf);

		// 	//	버퍼가 비었을 때 강제로 스코프 탈출시켜 if로 예외처리 되게 함
		// 	if (tmpBuf.empty())
		// 		break ;

		// 	Util::join(Body, tmpBuf, '\n');
		// 	contentLength -= tmpBuf.size();

		// 	if (chunkFlag)
		// 	{
		// 		Util::getline(buffer, tmpBuf);
		// 		contentLength = Util::to_hex(tmpBuf);
		// 	}

		// 	if (bodyMax && Body.size() > bodyMax)
		// 		throw statusCode = 413;
		// }
		// if (contentLength)
		// 	return READ_REQUEST;

		return DONE_REQUEST;
	}

	void makeBody()
	{
		if (!contentLength)
			return ;
	}

	void make_header(const std::string& buf)
	{
		std::stringstream ss(buf);
		std::string key;
		std::string val;

		std::getline(ss, key, ':');
		ss >> val;

		Header[key] = val;
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
		cout << "[Body]" << endl;
		cout << Body << endl;
	}

	bool parse_startline() {
		buffer >> StartLine.method >> StartLine.url >> StartLine.protocol;
		if (StartLine.method == "" || StartLine.url == "" || StartLine.protocol == "")
			return false;
		if (buffer.peek() != '\r' && buffer.peek() != '\n')
			return false;
		skip_crlf();
		return true;
	}

	bool is_done_buffer()
	{
		if (buffer.eof())
		{
			buffer.str("");
			buffer.clear();
			return true;
		}
		return false;
	}

	void skip_crlf()
	{
		if (buffer.peek() == '\r')
			buffer.get();
		if (buffer.peek() == '\n')
			buffer.get();
	}

	bool is_end_header()
	{
		if (buffer.peek() == '\r')
		{
			skip_crlf();
			return true;
		}
		return false;
	}
};

#endif //REQUEST_H
