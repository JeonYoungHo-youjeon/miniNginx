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
	RequestStartLine				StartLine;
	std::map<string, string>		Header;
	string							configName;
	std::stringstream				buffer;
	int 							statement;
	int 							progress;

	string virtualPath;
	string realPath;
	string resource;
	string excutor;
	string ext;

	int contentLength;
	bool chunkFlag;
	int statusCode;
	int clientFd;
	std::vector<string>	params;

	/**
	* @brief : 생성자 초기화, set()을 통한 초기화 등 임의로 다양하게 구현
	*/
	Request() : statusCode(200) {};
	Request(int fd, const string& configName) : statusCode(200), configName(configName), clientFd(clientFd) {};
	Request(const string& configName) : statusCode(200), configName(configName) {}

	void set(int fd, const string& configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		statement = eState::NONE;
		progress = REQ::START_LINE;
	}

	int read()
	{
		char rcvData[BUFFER_SIZE];
		int byte = recv(clientFd, &rcvData[0], BUFFER_SIZE, 0);

		if (byte <= 0)
		{
			strerror(errno);
			throw statusCode = 400;	//	catch (int statusCode);
		}
		buffer << rcvData;

		if (buffer.str().back() != '\n')
			return statement = eState::READ_REQUEST;

		return statement = parse();
	}

	int parse()
	{
		string tmpBuf;

		if (progress == REQ::START_LINE)
		{
			std::getline(buffer, tmpBuf, '\n');

			std::stringstream tmpSs(Util::remove_crlf(tmpBuf));
			tmpSs >> StartLine.method >> StartLine.url >> StartLine.protocol;
			if (!tmpSs)
				throw "Format Error";
			progress = REQ::HEADER;
		}

		while (progress == REQ::HEADER)
		{
			std::getline(buffer, tmpBuf);
			if (tmpBuf.back() != '\r')
			{
				buffer << tmpBuf;
				return eState::READ_REQUEST;
			}
			Util::strip(tmpBuf, '\r');
			if (tmpBuf.empty())
				progress = REQ::HEADER_SET;
			else
				makeHeader(tmpBuf);
		}

		if (progress == REQ::HEADER_SET)
		{
			virtualPath = Util::split(StartLine.url, '?')[0];
			params = Util::split(Util::split(StartLine.url, '?')[1], '&');

			if (Header.find("Transfer-Encoding") != Header.end())
				chunkFlag = true;
			if (Header.find("Content-Length") != Header.end())
				contentLength = Util::stoi(Header["Content-Length"]);

			if (chunkFlag ^ Header.find("Content-Length") != Header.end())
				throw "Request 116L Chunked && Content-Length";

			if (StartLine.method != "POST" && (progress = REQ::DONE))
				return eState::DONE_REQUEST;
		}

		while (progress == REQ::BODY)
		{
			std::getline(buffer, tmpBuf);

			if (tmpBuf == "\r")
				progress = REQ::DONE;
		}

		string maxSize = g_conf[configName][virtualPath]["client_max_body_size"][0];
		if (atoi(maxSize.c_str()) < contentLength)
			statusCode = 413;

		return eState::DONE_REQUEST;
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

	// FIXME: 에러는 throw 하는걸로 생각중인데, 우선 출력만 해놓고 에러 처리 방식 정해지면 다시 구현.
	/**
	* 받아온 리퀘스트 구조체에 한줄씩 들어오는 문자열을 상황에 맞게 처리해서 저장.
	*
	*/
	int set_request(string& str)
	{
		if (str.empty())
			throw std::logic_error("TEST");

		std::vector<string> crlf = Util::split(str, '\n');
		cout << "<" << crlf[0] << ">" << endl;
		int i = 0;
		cout << crlf.size() << endl;
		for (std::vector<std::string>::iterator it = crlf.begin(); it != crlf.end(); ++it)
		{

			if (callCount == 0)
			{
				virtualPath = Util::split(splited[1], '?')[0];
				StartLine.url = splited[1];
				params = Util::split(Util::split(splited[1], '?')[1], '&');

				//	.ext 찾기
				std::string::size_type pos = virtualPath.rfind('.');
				if (pos != static_cast<std::string::size_type>(-1))
					ext = std::string(virtualPath.begin() + pos, virtualPath.end());

				std::pair<std::string, std::string>	divpath = Util::divider(virtualPath, '/');
				while (divpath.first != "" && !g_conf[configName].is_exist(divpath.first))
					divpath = Util::divider(divpath, '/');

				virtualPath = divpath.first;
				resource = divpath.second;

				if (virtualPath.empty())
					virtualPath = "/";
				//	FIXME : host to Server IP:port
				if (g_conf[configName].is_exist(virtualPath))
				{
					if (g_conf[configName][virtualPath].is_exist("root"))
						realPath = g_conf[configName][virtualPath]["root"].front();
					if (g_conf[configName][virtualPath].is_exist(ext))
						excutor = g_conf[configName][virtualPath][ext].front();
				}
				else
					realPath = g_conf[configName].getAttr("root")[0];
			}
			else if (callCount == BODY)
			{
				if (StartLine.method != "POST")
					return 0;
				if (Header["encoding"] == "chunked")
				{
					//TODO: 청크드 메세지에서도 맥스 사이즈 확인
					switch (chunkState)
					{
						case HEAD:
						{
							long chunkSize = strtol((*it).c_str(), 0, 16);
							if (chunkSize == 0)
								return 0;
							remainString = chunkSize;
							chunkState = BODY;
						}
						// case BODY:
						// 	remainString = remainString - (*it).size();
						// 	if (remainString < 0)
						// 	{
						// 		*it = (*it).substr(0, remainString);
						// 		remainString = 0;
						// 	}
						// 	body += *it;
						// 	chunkState = HEAD;
					}
				}
				else
				{
					remainString = remainString - (*it).size();
					if (remainString < 0)
					{
						*it = (*it).substr(0, remainString);
						remainString = 0;
					}
					// body += *it + "\n";
				}
			}
			else
			{
				if (*it == "")
				{
					callCount = BODY;
					cout << "BODY start" << endl;
					continue ;
				}

				if (key == "Content-Length")
				{
					Header["Content-Length"] = Util::remove_crlf(splited[1]);
					remainString = atoi(Header["Content-Length"].c_str());
					try
					{
						//FIXME: 사용법 확인 후 수정

					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
					}
					continue ;
				}
			}
		}
		return remainString;
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
