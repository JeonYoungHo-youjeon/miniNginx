#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <cstdio>

# include "Cgi.hpp"
# include "File.hpp"
# include "../http.hpp"
# include "../parse/Util.hpp"

/*
 *  Http Test
 */
# include "../http.hpp"

extern Config g_conf;

struct ResponseStartLine
{
	string	protocol;		// HTTP 버전 (HTTP/1.1)
	int		statusCode;		// 상태 코드 (200, 404)
	string	reasonPhrase;	// 사유 구절
};

struct Response
{
	const Request&		Req;
	ResponseStartLine	startLine;
	map<string, string>	Header;
	string				Body;

	/**
	 * brief : 파일 경로 관련 변수
	 */
	Contents*	contentResult;
	string		path;
	string		filename;
	string		ext;
	string		excutor;
	vector<string>	params;

	Response(const Request& req);
	~Response();

	/**
	 *
	 * @return : Status
	 */
	int 	set()
	{
		if (Req.StartLine.method == "GET")
		{

		}
		if (Req.StartLine.method == "POST")
		{

		}
		if (Req.StartLine.method == "DELETE")
		{
			//if (remove(file path))
				throw StartLine.statusCode = 404;
			return 	DONE_RESPONSE;
		}
	}

	/**
	 * @brief :	Read & return nextToDo
	 * @return : Status
	 */
	int 	read()
	{
		//	TODO : 읽고 읽을것이 남아있으면 READ_RESPONSE 반환
	}

	/**
	 * Write & return nextTodo
	 * @return Status
	 */
	int 	write()
	{
		//	TODO : 쓰고 쓸 것이 남아있으면 WRITE_RESPONSE 반환
	}

	int 	execute()
	{
		//	TODO : execve or open, CGI라면 실행하고 자식 프로세스 outFd를 프로세스의 stdout으로 변경, 파일이라면 open하고 outFd를 파일의 fd로 변경

	}

	/**
	 * @brief : set -> execute -> read or write로 진행 예정
	 * @return set : EXECUTE || DONE
	 * @return exec : WRITE or READ
	 * @return write : WRITE or READ
	 * @return read : READ or DONE
	 */
	//			set() -> execute() -> write() for done -> read for done
	//	return	EXEC_ -> _READ || _WRITE for done
	/**
	 * @brief : 리스폰스 구조체의 변수들을 전송할 문자열로 변환하여 반환함
	 *
	 * @return string : 전송 데이터로 변환된 문자열
	 */
	 /*
	string get_response()
	{
		string ret;

		ret += get_protocol() + " ";
		ret += get_statusCode() + " ";
		ret += get_reasonPhrase() + "\n";
		ret = startLine.protocol + " " + std::to_string(startLine.statusCode) + " " + startLine.reasonPhrase + "\n";
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			ret += it->first + " " + it->second + "\n";
		ret += get_body();

		return ret;
	}
	  */

	string make_errorpage(int code) { return ""; }
	/*{
		body =
				"<!DOCTYPE html>\n"
				"<html>\n"
				"  <h1>\n"
				"    " + get_statusCode() + " " + get_reasonPhrase() + "\n"
																 "  </h1>\n"
																 "</html>\n";
		return get_response();
	}*/

	/**
	 * 현재 리스폰스 구조체의 내용 전체를 출력. 디폴트 값이 있는 데이터는 해당 값으로 출력
	 *
	 */
	void out()
	{
		cout << startLine.protocol << " " << startLine.statusCode << " " << startLine.reasonPhrase << "\n";
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			cout << it->first << " : " << it->second << endl;
		cout << "body" << endl;
		//cout << get_body() << endl;
	}

	/**
	 * 사유 구절이 있으면 해당 값 반환, 상태 코드가 없으면 공백 반환, 상태 코드가 있으면 해당 값에 맞게 반환.
	 *
	 */
	string get_reasonPhrase()
	{
		if (startLine.reasonPhrase != "")
			return startLine.reasonPhrase;

		switch (startLine.statusCode)
		{
			case 200:
				return "OK";
			case 404:
				return "NOT FOUND";

			default:
				return "DON'T_KNOW_THIS_CODE";
		}
	}
};
Response::Response(const Request& req)
:	Req(req)
{
	if ()
	//	이미 오류일 경우 실행 X
	if (startLine.statusCode != 200)
	{
		Body = make_errorpage(Req.statusCode);
		return ;
	}

	//if (g_conf)

	//	TMP
	//if (excutor.empty())
	//	contentResult = new File(path, filename, body, params);
	//else
	//	contentResult = new Cgi(path, filename, body, params, excutor);
}

Response::~Response()
{
	if (contentResult != nullptr)
		delete contentResult;
}
//int 	Response::set(){};
//int 	Response::run(){};
#endif