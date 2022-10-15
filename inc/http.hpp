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

	/**
	 * @brief 전체 리퀘스트가 하나의 문자열로 들어올때 처리. 테스트 용도로 따로 에러처리는 하지 않음
	 *
	 * @param str 리퀘스트를 통으로 담고있는 문자열
	 */
	Request(string str)
	{
	}

	/**
	* @brief 받아온 리퀘스트 구조체에 한줄씩 들어오는 문자열을 상황에 맞게 처리해서 저장, 에러는 throw 함
	* 
	* @param reqRef 문자열을 해석해서 저장할 리퀘스트 참조자
	* @param str 한줄씩 들어오는 실제 문자열
	*/
	void setRequest(Request& reqRef, string str)
	{
	}

	/**
	 * @brief 현재 리퀘스트 구조체의 내용 전체를 출력. 빈 변수는 출력하지 않음
	 * 
	 * @param reqRef 출력할 구조체
	 */
	void printRequest(Request& reqRef)
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

	/**
	 * @brief 리스폰스 참조자를 전송할 문자열로 변환하여 반환함
	 * 
	 * @param resRef 전송 데이터로 변환할 리스폰스 구조체
	 * @return string 전송 데이터로 변환된 문자열
	 */
	string getResponse(Response &resRef)
	{
	}

	/**
	 * @brief 현재 리스폰스 구조체의 내용 전체를 출력. 디폴트 값이 있는 데이터는 해당 값으로 출력
	 * 
	 * @param resRef 출력할 구조체
	 */
	void printResponse(Response& resRef)
	{
	}
};

#endif

