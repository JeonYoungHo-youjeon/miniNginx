#ifndef HTTP_HPP
# define HTTP_HPP

# include <algorithm>
# include <iostream>
# include <stdio.h>
# include <string>
# include <cstdlib>
# include <sys/time.h>
# include <sys/socket.h>

# include "./parse/Util.hpp"
# include "parse/Config.hpp"

# define BODY -1
# define BUFFER_SIZE 1024
# define HEAD 0

extern Config g_conf;

using std::string;
using std::cout;
using std::endl;

struct Request
{
	/* 
		GET /index.html HTTP/1.1
		hOSt: localhost


		POST /post/file1 HTTP/1.1
		Host: localhost:8085
		Content-Length: 12

		helloworld
	 */

	string method;			// GET, POST, DELETE 등의 메소드
	string url;				// 요청 URL (./ , /www/...)
	string protocol;		// HTTP 버전 (HTTP/1.1)
	string connention;		// 연결 옵션 (close 만 구현)
	string encoding;		// Transper-Encoding (chunked 만 구현)
	string host;			// 서버의 호스트 명과 포트 (localhost:80)
	string contentLength;	// 메세지가 담고있는 화물(엔터티entity = body) 의 길이
	string body;

	string location;
	string resource;
	string ext;
	int callCount;
	int remainString;
	int chunkState;
	int errorCode;
	vector<string>	params;

	Request() {};

	/**
	* 전체 리퀘스트가 하나의 문자열로 들어올때 처리. 따로 에러처리는 하지 않음
	*
	*/
	Request(string& str)
	{
		std::vector<string> splited = Util::split(str, '\n');

		for (std::vector<string>::size_type i = 0; i < splited.size(); i++)
		{
			set_request(splited[i]);
		}
	}

	string remove_crlf(string& str)
	{
		string ret;

		ret = Util::strip(str);
		ret = Util::remover(ret, '\r');
		return ret;
	}

	int set_request(int fd)
	{
		char rcvData[BUFFER_SIZE];
		int byte = recv(fd, &rcvData[0], BUFFER_SIZE, 0);
		if (byte <= 0)
		{
			strerror(errno);
			errorCode = 400;
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
					method = splited[0];
				else
					cout << "set_request ERROR 2 : " << splited[0] << ">" << endl;
				//	TODO
				//	내가 너무 많이 바꾸는 것 같아서 요청;;;;
				//	처음 들어온 url을 ?을 기준으로 나누고 첫 번째 걸 기존 location으로 생각하면 댐
				//	나머지는 & 기준으로 나누어서 벡터로 params를 만들어도 되고 ? 이후 문자열을 그냥 query 등으로 보관해도 ㄱㅊ
				url = splited[1];
				try
				{
					std::vector<string> splitUrl = Util::split(splited[1], '/');
					location = splited[1];

					//TODO: server 구해서 동적으로 넣어야함
					//FIXME: 사용법 이거 맞나?
					{
						std::string::size_type pos = location.rfind('.');
						if (pos != static_cast<std::string::size_type>(-1))
							ext = std::string(location.begin() + pos, location.end());
					}
					std::pair<std::string, std::string>	divpath = Util::divider(location, '/');
					while (divpath.first != "" && !g_conf["0.0.0.0:8000"].is_exist(divpath.first))
						divpath = Util::divider(divpath, '/');

					location = divpath.first;
					resource = divpath.second;
					url = g_conf["0.0.0.0:8000"][location]["root"][0];// + "/" + resource;
					std::vector<std::string>::iterator it = splitUrl.begin();
					it += 2;
					for (; it != splitUrl.end(); ++it)
					{
						url += "/" + *it;
					}
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				if (splited[2] == "HTTP/1.1")
				{
					protocol = remove_crlf(splited[2]);
				}
				else
					cout << "set_request ERROR 3 : " << splited[2] << ">" << endl;
			}
			else if (callCount == BODY)
			{
				if (method != "POST")
					return 0;
				if (encoding == "chunked")
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
				std::vector<string> splited = Util::split(*it, ' ');
				string lower = splited.at(0);

				std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
				std::cout << "lower: " <<*it << std::endl;
				if (lower == "connention:")
				{
					connention = remove_crlf(splited[1]);
					continue ;
				}

				if (lower == "encoding:")
				{
					encoding = remove_crlf(splited[1]);
					continue ;
				}

				if (lower == "host:")
				{
					host = remove_crlf(splited[1]);
					continue ;
				}

				if (lower == "Content-Length:")
				{
					contentLength = remove_crlf(splited[1]);
					remainString = atoi(contentLength.c_str());
					try
					{
						//FIXME: 사용법 확인 후 수정
						string maxSize = g_conf["0.0.0.0:8000"][location]["client_max_body_size"][0];
						if (g_conf["0.0.0.0:8000"][location]["client_max_body_size"][0] < contentLength)
						{
							errorCode = 413;
							// return ;
						}
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
					}
					continue ;
				}
				cout << "set_request ERROR 4 = <" << *it << ">" <<endl;
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
		cout << "method : <" 		<< method 			<< ">" << endl;
		cout << "url : <" 			<< url 				<< ">" << endl;
		cout << "protocol : <" 		<< protocol 		<< ">" << endl;
		cout << "connention : <"	<< connention 		<< ">" << endl;
		cout << "encoding : <" 		<< encoding 		<< ">" << endl;
		cout << "host : <" 			<< host 			<< ">" << endl;
		cout << "contentLength : <" << contentLength	<< ">" << endl;
		cout << "body : <" 			<< body 			<< ">" << endl;
		
	}
};

struct Response
{
	/*
		HTTP/1.1 500 Internal Server Error
		Connection: close
		Content-Length: 76
		Content-Type: text/html
		Date: Sat, 15 Oct 2022 14:32:11 GMT
		Server: webserv

		<!DOCTYPE html>
		<html>
		<h1>
			500 Internal Server Error
		</h1>
		</html>
		-------------------------------------
		HTTP/1.1 404 Not Found
		Content-Length: 64
		Content-Type: text/html
		Date: Sat, 15 Oct 2022 14:32:57 GMT
		Server: webserv

		<!DOCTYPE html>
		<html>
		<h1>
			404 Not Found
		</h1>
		</html>
		-------------------------------------
		HTTP/1.1 301 Moved Permanently
		Date: Sat, 15 Oct 2022 14:32:57 GMT
		Location: /
		Server: webserv
	 */
	
	string protocol;		// HTTP 버전 (HTTP/1.1)
	string statusCode;		// 상태 코드 (200, 404)
	string reasonPhrase;	// 사유 구절
	string date;			// 메세지가 만들어진 시간과 날짜
	string contentLength;	// 메세지가 담고있는 화물(엔터티entity = body) 의 길이
	string contentType;		// 메세지가 담고있는 화물 의 타입(text/html, image/jpeg)
	string encoding;		// Transper-Encoding (chunked 만 구현)
	string connection;		// 연결 옵션 (close 만 구현)
	string server;			// 서버 애플리케이션의 이름과 버전
	string location;		// 301 등에서 리소스의 위치를 알려줄때 사용

	string body;

	/**
	 * 리스폰스 구조체의 변수들을 전송할 문자열로 변환하여 반환함
	 * 
	 * @return string : 전송 데이터로 변환된 문자열
	 */
	string get_response()
	{
		string ret;

		ret += get_protocol() + " ";
		ret += get_statusCode() + " ";
		ret += get_reasonPhrase() + "\n";
		ret += "Date: " + get_date() + "\n";

		if (get_contentLength() != "")
			ret += "Content-Length: " + get_contentLength() + "\n";
		
		ret += "Content-Type: " + get_contentType() + "\n";

		if (get_encoding() != "")
			ret += "encoding: " + get_encoding() + "\n";
			
		if (get_connection() != "")
			ret += "connection: " + get_connection() + "\n";
		
		ret += "server: " + get_server() + "\n";

		if (get_location() != "") 
			ret += "location: " + get_location() + "\n";
		
		ret += "\n";
		ret += get_body();

		return ret; 
	}


	string make_errorpage(int code)
	{
		std::stringstream ss;
		ss << code;
		statusCode = ss.str();
		// FIXME: g_conf 순회하는 방법 찾고, open 하려면 어떻게 할지 문의
		// try
		// {
		// 	for (std::vector<string>::const_iterator it = g_conf["0.0.0.0:8000"][location]["error_page"].begin(); it != g_conf["0.0.0.0:8000"][location]["error_page"].end(); ++it)
		// 	{
		// 		if (*it == statusCode)
		// 		{
		// 			int fd = open(*(g_conf["0.0.0.0:8000"][location]["error_page"].end() - 1).c_str());
		// 			body = read(fd);
		// 			return get_response();
		// 		}
		// 	}
			
		// }
		// catch(const std::exception& e)
		// {
		// 	std::cerr << e.what() << '\n';
		// }
		

		body = 
		"<!DOCTYPE html>\n"
		"<html>\n"
		"  <h1>\n"
		"    " + statusCode + " " + get_reasonPhrase() + "\n"
		"  </h1>\n"
		"</html>\n";

		return get_response();
	}

	/**
	 * 현재 리스폰스 구조체의 내용 전체를 출력. 디폴트 값이 있는 데이터는 해당 값으로 출력
	 *
	 */
	void print_response()
	{
		cout << "protocol : <" 		<< get_protocol() 		<< ">" << endl;
		cout << "statusCode : <" 	<< get_statusCode() 	<< ">" << endl;
		cout << "reasonPhrase : <" 	<< get_reasonPhrase() 	<< ">" << endl;
		cout << "date : <" 			<< get_date() 			<< ">" << endl;
		cout << "contentLength : <" << get_contentLength()	<< ">" << endl;
		cout << "contentType : <" 	<< get_contentType() 	<< ">" << endl;
		cout << "encoding : <" 		<< get_encoding()		<< ">" << endl;
		cout << "connection : <" 	<< get_connection() 	<< ">" << endl;
		cout << "server : <" 		<< get_server() 		<< ">" << endl;
		cout << "location : <" 		<< get_location() 		<< ">" << endl;
		cout << "body : <" 			<< get_body() 			<< ">" << endl;
	}

	/**
	 * 프로토콜이 있으면 해당값, 없으면 "HTTP/1.1" 반환
	 * 
	 */
	string get_protocol()
	{
		if (protocol != "")
			return protocol;
		
		return ("HTTP/1.1");
	}

	/**
	 * 상태 코드를 반환. 기본값 없음
	 * 
	 */
	string get_statusCode()
	{
		return statusCode;
	}

	/**
	 * 사유 구절이 있으면 해당 값 반환, 상태 코드가 없으면 공백 반환, 상태 코드가 있으면 해당 값에 맞게 반환.
	 * 
	 */
	string get_reasonPhrase()
	{
		if (reasonPhrase != "")
			return reasonPhrase;
		
		if (statusCode == "")
			return "";
		
		switch (std::atoi(statusCode.c_str())) 
		{
			case 200:
				return "OK";
			case 404:
				return "NOT FOUND";

			default:
				return "DON'T_KNOW_THIS_CODE";
		}
	}

	/**
	 * 현재 시간을 이용하여 HTTP 헤더에 맞는 값으로 변환하여 반환. 
	 *
	 */
	string get_date()
	{
		time_t rawtime;
		struct tm *timeinfo;
		char buf[31];

		time(&rawtime);
		timeinfo = gmtime(&rawtime);
		strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
		date = buf;

		return date;
	}

	/**
	 * 엔터티의 길이를 반환. encoding 이 chunked 일때는 공백을 반환. contentLength 가 비어있으면 body의 size 반환.
	 * 
	 */
	string get_contentLength()
	{
		if (encoding == "chunked")
			return ("");
		if (contentLength == "")
		{
			std::stringstream ss;
			ss << body.size();
			contentLength = ss.str();
		}
		return contentLength;
	}

	/**
	 * 엔터티의 타입을 반환. 기본 값을 text/html 로 할까 하다가 "application/octet-stream" 를 임시로 넣어둠(크롬에서 파일 다운로드 됨.)
	 * 
	 */
	string get_contentType()
	{
		if (contentType == "")
			return ("application/octet-stream");
		return contentType;
	}

	/**
	 * encoding 을 반환. 이 과제에서는 chunked 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음 
	 * 
	 */
	string get_encoding()
	{
		return encoding;
	}

	/**
	 * connection 을 반환. 이 과제에서는 close 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음 
	 * 
	 */
	string get_connection()
	{
		return connection;
	}

	/**
	 * server 를 반환. 기본값은 "miniNginx 1.0"
	 * 
	 */
	string get_server()
	{
		if (server == "")
			return ("miniNginx 1.0");
		return server;
	}

	/**
	 * location 을 반환. 기본값 없음
	 * 
	 */
	string get_location()
	{
		return location;
	}

	/**
	 * body를 반환. 기본값 없음 
	 *
	 */
	string get_body()
	{
		return body;
	}

};

#endif

