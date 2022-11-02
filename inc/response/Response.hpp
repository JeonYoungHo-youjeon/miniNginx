#ifndef RESPONSE_HPP
# define RESPONSE_HPP

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
	ResponseStartLine(int code) : statusCode(code) {};
	string	protocol;		// HTTP 버전 (HTTP/1.1)
	int		statusCode;		// 상태 코드 (200, 404)
	string	reasonPhrase;	// 사유 구절
};

struct Response
{
	ResponseStartLine	startLine;
	map<string, string>	Header;

	string body;

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
	 * @brief : 리스폰스 구조체의 변수들을 전송할 문자열로 변환하여 반환함
	 *
	 * @return string : 전송 데이터로 변환된 문자열
	 */
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

	string make_errorpage(int code)
	{
		startLine.statusCode = code;

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
				"    " + get_statusCode() + " " + get_reasonPhrase() + "\n"
																 "  </h1>\n"
																 "</html>\n";
		return get_response();
	}

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
		cout << get_body() << endl;
	}

	/**
	 * 프로토콜이 있으면 해당값, 없으면 "HTTP/1.1" 반환
	 *
	 */
	string get_protocol()
	{
		if (Header.find("protocol") != Header.end())
			return Header["protocol"];
		return ("HTTP/1.1");
	}

	/**
	 * 상태 코드를 반환. 기본값 없음
	 *
	 */
	string get_statusCode()
	{
		return std::to_string(startLine.statusCode);
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
		Header["date"] = buf;

		return Header["date"];
	}

	/**
	 * 엔터티의 길이를 반환. encoding 이 chunked 일때는 공백을 반환. contentLength 가 비어있으면 body의 size 반환.
	 *
	 */
	string get_contentLength()
	{
		if (Header.find("Transfer-Encoding") == Header.end() && Header["Transfer-Encoding"] == "chunked")
			return "";
		if (Header.find("Content-Length") == Header.end())
			return std::to_string(body.size());
		return "";
	}

	/**
	 * 엔터티의 타입을 반환. 기본 값을 text/html 로 할까 하다가 "application/octet-stream" 를 임시로 넣어둠(크롬에서 파일 다운로드 됨.)
	 *
	 */
	string get_contentType()
	{
		if (Header.find("Content-Type") == Header.end())
			return ("application/octet-stream");
		return Header["Content-Type"];
	}

	/**
	 * encoding 을 반환. 이 과제에서는 chunked 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음
	 *
	 */
	string get_encoding()
	{
		return Header["encoding"];
	}

	/**
	 * connection 을 반환. 이 과제에서는 close 유무여서 해당 변수를 bool 로 할까 하다가 우선 string 으로 뒀음
	 *
	 */
	string get_connection()
	{
		return Header["connection"];
	}

	/**
	 * server 를 반환. 기본값은 "miniNginx 1.0"
	 *
	 */
	string get_server()
	{
		if (Header.find("server") == Header.end())
			return "miniNginx 1.0";
		return Header["server"];
	}

	/**
	 * location 을 반환. 기본값 없음
	 *
	 */
	string get_location()
	{
		if (Header.find("location") == Header.end())
			return "";
		return Header["location"];
	}

	/**
	 * body를 반환. 기본값 없음
	 *
	 */
	string get_body()
	{
		return contentResult->getBody();
	}
	//int 	set();
	//int 	run();
};
Response::Response(const Request& req)
: startLine(req.statusCode),
	contentResult(nullptr),
	params(req.params),
	path(req.realPath),
	filename(req.resource),
	ext(req.ext),
	excutor(req.excutor)
	// body(req.body)
{
	Header["host"] = req.Header.find("host")->second;
	//	이미 오류일 경우 실행 X
	if (startLine.statusCode != 200)
		return ;

	//	TMP
	body = "TEST1\nTEST2\n";

	if (excutor.empty())
		contentResult = new File(path, filename, body, params);
	else
		contentResult = new Cgi(path, filename, body, params, excutor);
	if (req.StartLine.method == "GET")
		contentResult->_get();
	else if (req.StartLine.method == "POST")
		contentResult->_post();
	else if (req.StartLine.method == "DELETE")
		contentResult->_delete();
	body = get_body();
}
Response::~Response()
{
	if (contentResult != nullptr)
		delete contentResult;
}
//int 	Response::set(){};
//int 	Response::run(){};
#endif