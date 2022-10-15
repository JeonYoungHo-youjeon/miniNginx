#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <sys/time.h>

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

	/**
	* 전체 리퀘스트가 하나의 문자열로 들어올때 처리. 테스트 용도로 에러처리는 하지 않음
	*
	*/
	Request(string str)
	{
	}

	/**
	* 받아온 리퀘스트 구조체에 한줄씩 들어오는 문자열을 상황에 맞게 처리해서 저장. 에러는 throw 함
	* 
	*/
	void set_request(string str)
	{
	}

	/**
	*  현재 리퀘스트 구조체의 내용 전체를 출력. 빈 변수는 출력하지 않음
	* 
	*/
	void print_request()
	{

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
		if (this->protocol != "")
			return this->protocol;
		
		return ("HTTP/1.1");
	}

	/**
	 * 상태 코드를 반환. 기본값 없음
	 * 
	 */
	string get_statusCode()
	{
		return this->statusCode;
	}

	/**
	 * 사유 구절이 있으면 해당 값 반환, 상태 코드가 없으면 공백 반환, 상태 코드가 있으면 해당 값에 맞게 반환.
	 * 
	 */
	string get_reasonPhrase()
	{
		if (this->reasonPhrase != "")
			return this->reasonPhrase;
		
		if (this->statusCode == "")
			return "";
		
		switch (std::atoi(this->statusCode.c_str())) 
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
		this->date = buf;

		return this->date;
	}

	/**
	 * 엔터티의 길이를 반환. encoding 이 chunked 일때는 공백을 반환
	 * 
	 */
	string get_contentLength()
	{
		if (this->encoding == "")
			return ("");
		if (this->contentLength == "")
			return ("0");
		return this->contentLength;
	}

	/**
	 * 엔터티의 타입을 반환. 기본 값을 text/html 로 할까 하다가 "application/octet-stream" 를 임시로 넣어둠(크롬에서 파일 다운로드 됨.)
	 * 
	 */
	string get_contentType()
	{
		if (this->contentType == "")
			return ("application/octet-stream");
		return this->contentType;
	}

	/**
	 * encoding 을 반환. 이 과제에서는 chunked 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음 
	 * 
	 */
	string get_encoding()
	{
		return this->encoding;
	}

	/**
	 * connection 을 반환. 이 과제에서는 close 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음 
	 * 
	 */
	string get_connection()
	{
		return this->connection;
	}

	/**
	 * server 를 반환. 기본값은 "miniNginx 1.0"
	 * 
	 */
	string get_server()
	{
		if (this->server == "")
			return ("miniNginx 1.0");
		return this->server;
	}

	/**
	 * location 을 반환. 기본값 없음
	 * 
	 */
	string get_location()
	{
		return this->location;
	}

	/**
	 * body를 반환. 기본값 없음 
	 *
	 */
	string get_body()
	{
		return this->body;
	}

};

#endif

