#ifndef HTTP_HPP
# define HTTP_HPP

# include <algorithm>
# include <cctype>
# include <iostream>
# include <string>
# include <cstdlib>
# include <sys/time.h>

# include "./parse/Util.hpp"

# define BODY -1

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

	int callCount;

	Request();

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

	// FIXME: 에러는 throw 하는걸로 생각중인데, 우선 출력만 해놓고 에러 처리 방식 정해지면 다시 구현.
	/**
	* 받아온 리퀘스트 구조체에 한줄씩 들어오는 문자열을 상황에 맞게 처리해서 저장. 
	* 
	*/
	void set_request(string str)
	{
		// callCount 가 0 일때 = 처음 호출됨 = 메소드, url, 프로토콜 저장
		// callCount 가 -1(BODY)일때 = body 구간. 입력되는 만큼 body에 계속 저장함.
		// callCount 가 0 보다 클때 = 헤더 필드 구간. 공백이 나오면 callCount 를 BODY 로 바꾸고, 그렇지 않으면 구조체에 필드 저장
		
		
		if (callCount == 0)
		{
			std::vector<string> splited = Util::split(str, ' ');
			callCount++;
			if (splited.size() != 3)
				cout << "set_request ERROR 1" << endl;

			if (splited[0] == "GET" || splited[0] == "POST" || splited[0] == "DELETE")
				method = splited[0];
			else
				cout << "set_request ERROR 2" << endl;

			url = splited[1];

			if (splited[2] == "HTTP/1.1")
				protocol = splited[2];
			else
				cout << "set_request ERROR 3" << endl;
		}
		else if (callCount == BODY)
		{
			body += str;
		}
		else
		{
			std::vector<string> splited = Util::split(str, ':');
			string lower = splited[0];

			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
			if (str == "\n")
			{
				callCount = BODY;
				return ;
			}

			if (lower == "connention")
			{
				connention = splited[1];
				connention.erase(connention[0]);
				return ;
			}

			if (lower == "encoding")
			{
				encoding = splited[1];
				encoding.erase(encoding[0]);
				return ;
			}

			if (lower == "host")
			{
				host = splited[1];
				host.erase(host[0]);
				return ;
			}

			if (lower == "Content-Length")
			{
				contentLength = splited[1];
				contentLength.erase(contentLength[0]);
				return ;
			}

			cout << "set_request ERROR 4" << endl;
		}

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
	 * 엔터티의 길이를 반환. encoding 이 chunked 일때는 공백을 반환
	 * 
	 */
	string get_contentLength()
	{
		if (encoding == "")
			return ("");
		if (contentLength == "")
			return ("0");
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

