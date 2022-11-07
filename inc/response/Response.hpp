#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <cstdio>
# include <sstream>

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
	const Request*		Req;
	ResponseStartLine	StartLine;
	map<string, string>	Header;
	string				Body;

	string				configName;
	string				postBody;
	int					contentLength;
	int					statement;
	int					progress;

	/**
	 * brief : 파일 경로 관련 변수
	 */
	Contents*		contentResult;
	string			path;
	string			fileName;
	string			ext;
	string			excutor;
	vector<string>	params;

	Response();
	Response(const Request& req);
	~Response();

	int set(const std::string& configName, int error_code)
	{
		this->configName = configName;
		StartLine.statusCode = error_code;
		return execute();
	}
	int set(const Request& req)
	{
		Req = &req;
		postBody = req.buffer.str();
		path = req.locationName;
		fileName = req.fileName;
		if (!req.locationName.empty() && g_conf[Req->configName][path].is_exist("root"))
			path = g_conf[Req->configName][path]["root"][0];
		path += fileName;
		ext = req.ext;

		progress = READY;
		return statement = execute();
	}
	/**
	 * @brief Set & return nextToDo
	 * @return statement
	 */
	int 	execute();

	/**
	 * @brief Write & return nextToDo
	 * @return statement
	 */
	int 	write();

	/**
	 * @brief Read & return nextToDo
	 * @return statement
	 */
	int 	read();

	/**
	 * @brief : exec -> read or write로 진행 예정
	 * @return exec : WRITE or READ or DONE
	 * @return write : WRITE or READ
	 * @return read : READ or DONE
	 */

	//			set() -> execute() -> write() for done -> read for done
	//	return	EXEC_ -> _READ || _WRITE for done

	int		makeHeader()
	{
		//	TODO : 이제 octet, contentLength, connection ... 처리
		Header["Content-Length"] = Util::to_string(Body.size());
		Header["Date"] = Util::get_date();
		Header["Connection"] = "Keep-Alive";
		Header["Server"] = "miniNginx/1.1";
		{    /* 필요 헤더*/    }
		return makeStartLine();
	}

	int		makeStartLine()
	{
		StartLine.reasonPhrase = get_reasonPhrase();
		StartLine.protocol = "HTTP/1.1";
		return statement = DONE_RESPONSE;
	}

	int make_errorpage(int code)
	{
		Body =
				"<!DOCTYPE html>\n"
				"<html>\n"
				"  <h1>\n"
				"    " + Util::to_string(StartLine.statusCode) + " " + get_reasonPhrase() + "\n"
				"  </h1>\n"
				"</html>\n";
		return makeHeader();
	}

	/**
	 * 사유 구절이 있으면 해당 값 반환, 상태 코드가 없으면 공백 반환, 상태 코드가 있으면 해당 값에 맞게 반환.
	 *
	 */

	string get_reasonPhrase()
	{
		if (!StartLine.reasonPhrase.empty())
			return StartLine.reasonPhrase;

		switch (StartLine.statusCode)
		{
			case 200:
				return "OK";
			case 201:
				return "Created";
			case 204:
				return "No Content";
			case 301:
				return "Moved Permanently";
			case 400:
				return "Bad Request";
			case 403:
				return "Forbidden";
			case 404:
				return "Not Found";
			case 405:
				return "Method Not Allowed";
			case 413:
				return "Payload Too Large";
			case 500:
				return "Internal Server Error";

			default:
				return "DON'T_KNOW_THIS_CODE";
		}
	}
};

Response::Response()
{}


Response::~Response()
{
	if (contentResult != nullptr)
		delete contentResult;
}

int 	Response::execute()
{
	/**
	 * @brief try remove file, throw error if catch error 
	 */
	
	try
	{
		if (StartLine.statusCode != 200)
			throw StartLine.statusCode;

		if (Req->StartLine.method == "DELETE")
		{
			if (remove(path.c_str()))
				throw StartLine.statusCode = 404;
			return 	makeHeader();
		}
  
		if (g_conf[Req->configName][Req->locationName].is_exist(ext))
			contentResult = new Cgi(path, ext, params);
		else
			contentResult = new File(path);
		
		progress = contentResult->set();

		if (Req->StartLine.method == "GET")
			return statement = READ_RESPONSE;

		if (Req->StartLine.method == "POST")
			return statement = WRITE_RESPONSE;
		
		//	Method Error -> Bad Request
		throw StartLine.statusCode = 400;
	}
	catch (int errNo)	//	예외 발생 시 일단 객체 내에서 처리 -> 수정 O
	{
		return make_errorpage(errNo);
	}
	return makeHeader();
}

int 	Response::write()
{
	if (postBody.empty())
		return READ_RESPONSE;

	//	쓰고 쓸 것이 남아있으면 WRITE_RESPONSE 반환
	size_t	len = ::write(contentResult->inFd, postBody.c_str(), BUFFER_SIZE);

	if (len < 0)
		throw StartLine.statusCode = 500;
	else if (len == 0)
		return makeHeader();

	postBody.erase(0, len);
	//	전부 보냈으면 결과 값 받아오기 위해 READ_RESPONSE 반환
	if (len < BUFFER_SIZE)
		return READ_RESPONSE;
	return WRITE_RESPONSE;
}

int 	Response::read()
{
	//	읽고 읽을것이 남아있으면 READ_RESPONSE 반환
	char buf[BUFFER_SIZE];
	size_t	len = ::read(contentResult->outFd, buf, BUFFER_SIZE);

	Body += string(buf, len);
	//	TODO : READ 탈출 조건 정하기 - 일단 무조건 READ_RESPONSE 보내고 클라이언트에서 kill & close 하기?
	if (!len)
		return makeHeader();
	if (len < 0)
		throw StartLine.statusCode = 500;
	
	return READ_RESPONSE;
}

#endif