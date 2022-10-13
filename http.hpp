#ifndef HTTP_HPP
# define HTTP_HPP

# include <string>

using std::string;

typedef struct Request
{
	/*
		GET /redirect HTTP/1.1
		Host: localhost
		content-length: 79
		connection : close

		hello world
		this is message body
		all newline characters
		replaced by crlf
	*/

	string method;			// GET, POST, DELETE
	string URL;				// 요청 URL (/, /www...) 
	string protocol;		// HTTP 버전 (HTTP/1.1)
	string connection;		// 연결 옵션 (Close 만 구현)
	string encoding;		// Transper-Encoding (chunked 만 구현)
	string host;			// 서버의 호스트 명과 포트 (localhost:80)
	string contentLength;	// 리퀘스트 메세지가 담고있는 화물(entity 엔터티 = body)의 길이
	
	string body;
} Request;

typedef struct Reponse 
{
	/*
		HTTP/1.1 301 Moved Permanently
		Date: Thu, 13 Oct 2022 12:21:38 GMT
		Location: /
		Server: webserv

		HTTP/1.1 200 OK
		Connection: close
		Content-Length: 15
		Content-Type: text/html
		Date: Thu, 13 Oct 2022 12:21:38 GMT
		Server: webserv

		<html>
		</html>
	*/

	string protocol;		// HTTP 버전 (HTTP/1.1)
	string statusCode;		// 상태 코드 (200, 404)
	string reasonPhrase;	// 사유 구절 (OK, NOT FOUND)
	string date;			// 메세지가 만들어진 시간과 날짜
	string contentLength;	// 리퀘스트 메세지가 담고있는 엔터티 = body의 길이
	string contentType;		// 리퀘스트 메세지가 담고있는 엔터티 = body의 타입 (text/html)
	string encoding;		// Transper-Encoding (chunked 만 구현)
	string connection;		// 연결 옵션 (Close 만 구현)
	string server;			// 서버 애플리케이션의 이름과 버전
	string location;		// 301 등에서 리소스의 위치를 알려줄때 사용

	string body;
} Reponse;

#endif