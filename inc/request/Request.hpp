#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sys/socket.h>

# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../Body.hpp"

using std::string, std::cout , std::endl;

extern Config g_conf;

const int BUFFER_SIZE = 1024;
enum e_parsePosition
{
	BODY = -1,
	HEAD = 0
};

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

	Body body;

	string virtualPath;
	string realPath;
	string resource;
	string excutor;
	string ext;

	int callCount;
	int remainString;
	int chunkState;
	int statusCode;
	string configName;
	std::vector<string>	params;

	Request() : statusCode(200) {};

	/**
	* 전체 리퀘스트가 하나의 문자열로 들어올때 처리. 따로 에러처리는 하지 않음
	*
	*/
	//	FIXME	: HOST가 0.0.0.0:8000 리터럴로 고정되어있음
	Request(string& str) : statusCode(200)
	{
		std::vector<string> splited = Util::split(str, '\n');
		for (std::vector<string>::size_type i = 0; i < splited.size(); i++)
			set_request(splited[i]);
	}

	string remove_crlf(string& str)
	{
		string ret;

		ret = Util::strip(str);
		ret = Util::remover(ret, '\r');
		return ret;
	}

	int set_request(int fd, string ip)
	{
		char rcvData[BUFFER_SIZE];
		int byte = recv(fd, &rcvData[0], BUFFER_SIZE, 0);
		configName = ip;
		if (byte <= 0)
		{
			strerror(errno);
			statusCode = 400;
			return -1;
		}

		string data = rcvData;
		return set_request(data);
	}


	// FIXME: 에러는 throw 하는걸로 생각중인데, 우선 출력만 해놓고 에러 처리 방식 정해지면 다시 구현.
	/**
	* 받아온 리퀘스트 구조체에 한줄씩 들어오는 문자열을 상황에 맞게 처리해서 저장.
	*
	*/
	int set_request(string& str)
	{
		// callCount 가 0 일때 = 처음 호출됨 = 메소드, url, 프로토콜 저장
		// callCount 가 -1(BODY)일때 = body 구간. 입력되는 만큼 body에 계속 저장함.
		// callCount 가 0 보다 클때 = 헤더 필드 구간. 공백이 나오면 callCount 를 BODY 로 바꾸고, 그렇지 않으면 구조체에 필드 저장
		if (str.empty())
			throw std::logic_error("TEST");

		std::vector<string> crlf = Util::split(str, '\n');
		cout << "<" << crlf[0] << ">" << endl;
		int i = 0;
		cout << crlf.size() << endl;
		for (std::vector<std::string>::iterator it = crlf.begin(); it != crlf.end(); ++it)
		{
			*it = Util::remover(*it, '\r');
			cout << i++ << endl;

			cout << "for <" << *it << ">" << endl;
			if (callCount == 0)
			{
				std::vector<string> splited = Util::split(*it, ' ');
				if (splited.size() != 3)
				{
					cout << "set_request ERROR 1 : <" << str << ">" << endl;
					throw std::logic_error("TEST");
				}
				else
					callCount++;

				if (splited[0] == "GET" || splited[0] == "POST" || splited[0] == "DELETE")
					StartLine.method = splited[0];
				else
					cout << "set_request ERROR 2 : " << splited[0] << ">" << endl;
				//	FIXME
				Header["host"] = "0.0.0.0:8000";
				virtualPath = Util::split(splited[1], '?')[0];
				params = Util::split(Util::split(splited[1], '?')[1], '&');

				//	.ext 찾기
				std::string::size_type pos = virtualPath.rfind('.');
				if (pos != static_cast<std::string::size_type>(-1))
					ext = std::string(virtualPath.begin() + pos, virtualPath.end());

				std::pair<std::string, std::string>	divpath = Util::divider(virtualPath, '/');
				while (divpath.first != "" && !g_conf[Header["host"]].is_exist(divpath.first))
					divpath = Util::divider(divpath, '/');

				virtualPath = divpath.first;
				resource = divpath.second;
				if (virtualPath.empty())
					virtualPath = "/";
				if (g_conf[Header["host"]].is_exist(virtualPath))
				{
					if (g_conf[Header["host"]][virtualPath].is_exist("root"))
						realPath = g_conf[Header["host"]][virtualPath]["root"].front();
					if (g_conf[Header["host"]][virtualPath].is_exist(ext))
						excutor = g_conf[Header["host"]][virtualPath][ext].front();
				}
				else
					realPath = g_conf[Header["host"]].getAttr("root")[0];

				if (splited[2] == "HTTP/1.1")
					StartLine.protocol = remove_crlf(splited[2]);
				else
					cout << "set_request ERROR 3 : " << splited[2] << ">" << endl;
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
						case BODY:
							remainString = remainString - (*it).size();
							if (remainString < 0)
							{
								*it = (*it).substr(0, remainString);
								remainString = 0;
							}
							body += *it;
							chunkState = HEAD;
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
					body += *it + "\n";
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
				std::vector<string> splited = Util::split(*it, ':');
				string key = remove_crlf(splited.at(0));
				string value = remove_crlf(splited.at(1));
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				std::cout << "Key : " <<*it << std::endl;
				Header[key] = value;
				if (key == "Content-Length")
				{
					Header["Content-Length"] = remove_crlf(splited[1]);
					remainString = atoi(Header["Content-Length"].c_str());
					try
					{
						//FIXME: 사용법 확인 후 수정
						string maxSize = g_conf[configName][virtualPath]["client_max_body_size"][0];
						if (atoi(maxSize.c_str()) < remainString)
							statusCode = 413;
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
		for (auto it = Header.begin(); it != Header.end(); ++it)
			cout << it->first << " : " << it->second << endl;
	}
};

#endif //REQUEST_H
