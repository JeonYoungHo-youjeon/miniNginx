#ifndef HTTP_HPP
# define HTTP_HPP

# include <string>

typedef struct Request
{
	/*
		
	*/
	std::string method;			// GET, POST, DELETE
	std::string URL;			// 요청 URL (/, /www...) 
	std::string protocol;		// HTTP 버전 (HTTP/1.1)
	std::string connection;		// 연결 옵션 (Close 만 구현)
	std::string encoding;		// Transper-Encoding (chunked 만 구현)
	std::string host;			// 서버의 호스트 명과 포트 (localhost:80)
	std::string contentLength;	// 리퀘스트 메세지가 담고있는 화물(entity 엔터티 = body)의 길이
	
	std::string body;
} Request;

typedef struct Reponse 
{
	/*
		
	*/
	std::string protocol;		// HTTP 버전 (HTTP/1.1)
	std::string statusCode;		// 상태 코드 (200, 404)
	std::string reasonPhrase;	// 사유 구절 (OK, NOT FOUND)
	std::string date;			// 메세지가 만들어진 시간과 날짜
	std::string contentLength;	// 리퀘스트 메세지가 담고있는 엔터티 = body의 길이
	std::string contentType;	// 리퀘스트 메세지가 담고있는 엔터티 = body의 타입 (text/html)
	std::string encoding;		// Transper-Encoding (chunked 만 구현)
	std::string connection;		// 연결 옵션 (Close 만 구현)
	std::string server;			// 서버 애플리케이션의 이름과 버전
	std::string location;		// 301 등에서 리소스의 위치를 알려줄때 사용

	std::string body;
} Reponse;

#endif