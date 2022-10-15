#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>
# include <string>

# define BODY -1

using std::string;

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

	Request(string str)
	{
	}

	void setRequest(Request& req, string str)
	{
	}

	void printRequest(Request& req)
	{
	}
};

struct Response
{
	/*
		
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

	string getResponse(Response &res)
	{
	}

	void printResponse(Response& res)
	{
	}

};

#endif

