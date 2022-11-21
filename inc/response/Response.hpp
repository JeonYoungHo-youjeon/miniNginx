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
	Request *Req;
	ResponseStartLine StartLine;
	map<string, string> Header;
	string Body;

	string* Html;

	string configName;
	string postBody;
	int contentLength;
	int statement;
	int progress;

	/**
	 * brief : 파일 경로 관련 변수
	 */
	Contents *contentResult;
	string locationName;
	string path;
	string fileName;
	string ext;
	string excutor;
	vector<string> params;

	Response();

	Response(const Request & req);

	~Response();

	string toHtml()
	{
		string ret;
		ret += StartLine.protocol + ' ' + Util::to_string(StartLine.statusCode) + ' ' + StartLine.reasonPhrase + "\r\n";
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			ret += it->first + ": " + it->second + "\r\n";
		ret += "\r\n";
		if (!Body.empty())
			ret += Body + "\r\n";
		return ret;
	}
	int clear();

	int set(const std::string& configName, int error_code)
	{
		this->configName = configName;
		StartLine.statusCode = error_code;
		return make_errorpage(error_code);
	}

	int set(const Request& req)
	{
		StartLine.statusCode = 200;
		Req = &req;
		postBody = req.buffer.str();
		locationName = path = req.locationName;
		fileName = req.fileName;
		if (!req.locationName.empty() && g_conf[Req->configName][path].is_exist("root"))
			path = g_conf[Req->configName][path]["root"][0];
		path = Util::join(path, fileName, '/');
		ext = req.ext;

		progress = READY;
		return statement = execute();
	}

	/**
	 * @brief Set & return nextToDo
	 * @return statement
	 */
	int execute();

	/**
	 * @brief Write & return nextToDo
	 * @return statement
	 */
	int write();

	/**
	 * @brief Read & return nextToDo
	 * @return statement
	 */
	int read();

	int send(int clientFd);

	/**
	 * @brief : exec -> read or write로 진행 예정
	 * @return exec : WRITE or READ or DONE
	 * @return write : WRITE or READ
	 * @return read : READ or DONE
	 */

	int makeHeader()
	{
		std::cout << "OK" << std::endl;
		Header["Date"] = Util::get_date();
		Header["Server"] = "miniNginx/1.1";

		map<string, string>::iterator it = Header.find("Connection");
		if (it == Header.end())
			Header["Connection"] = "keep-alive";
		Header["Content-Type"] = "Text/html";
		Header["Content-Type"] = g_conf.getContentType(ext);
		Header["Content-Length"] = Util::to_string(Body.size());
		{    /* 필요 헤더*/    }
		return makeStartLine();
	}

	int makeStartLine()
	{
		StartLine.reasonPhrase = g_conf.getStatusMsg(StartLine.statusCode);
		StartLine.protocol = "HTTP/1.1";
		std::cout << "STARTLINE OK" << std::endl;
		return statement = SEND_RESPONSE;
	}

	int make_errorpage(int code)
	{
		ext = ".html";
		if (g_conf[configName][locationName].is_exist("error_page"))
		{
			for (size_t i = 0; i < g_conf[configName][locationName]["error_page"].size() - 1; ++i)
				if (Util::to_string(StartLine.statusCode) == g_conf[configName][locationName]["error_page"][i])
					path = g_conf[configName][locationName]["error_page"].back();
			return execute();
		}
		Body =
				"<!DOCTYPE html>\n"
				"<html>\n"
				"  <h1>\n"
				"    " + Util::to_string(StartLine.statusCode) + "\n"
				"  </h1>\n"
				"</html>\n";
		return makeHeader();
	}
};

Response::Response()
{}
Response::~Response()
{
	clear();
}

int 	Response::execute()
{
	/**
	 * @brief try remove file, throw error if catch error 
	 */
	
	try
	{
		if (Req->StartLine.method == "DELETE")
		{
			if (remove(path.c_str()))
				throw 404;
			Body = path;
			return 	makeHeader();
		}
		//	Method Error -> Bad Request
		if (Req->StartLine.method != "GET" && Req->StartLine.method != "POST")
			throw 400;

		if (g_conf[Req->configName][Req->locationName].is_exist(ext))
		{
			if (Req->cookies.count(ext) == 0)
			{
				Header["set-cookie"] = ext + "=" + Req->session.gen_random(12) + ";";
			}
			else
			{
				string tmp;
				for (std::vector<string>::iterator it = Req->session.Session[Req->cookies[ext]].begin(); it != Req->session.Session[Req->cookies[ext]].end(); ++it)
					tmp += *it + ":";
				if (!tmp.empty())
				{
					tmp.insert(0, "COOKIE=");	
					tmp.erase(tmp.size() - 1);
					params.push_back(tmp);
				}
			}

			contentResult = new Cgi(path, ext, params);
		}
		else
			contentResult = new File(path);

		progress = contentResult->set();

		if (Req->StartLine.method == "GET")
			return statement = READ_RESPONSE;

		if (Req->StartLine.method == "POST")
			return statement = WRITE_RESPONSE;
	}
	catch (int errNo)	//	예외 발생 시 일단 객체 내에서 처리 -> 수정 O
	{
		return make_errorpage(StartLine.statusCode = errNo);
	}
	return makeHeader();
}

int 	Response::write()
{
	if (postBody.empty())
		return READ_RESPONSE;

	//	쓰고 쓸 것이 남아있으면 WRITE_RESPONSE 반환
	ssize_t	len = ::write(contentResult->inFd, postBody.c_str(), BUFFER_SIZE);

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
	char buf[BUFFER_SIZE] = {0};
	ssize_t	len = ::read(contentResult->outFd, buf, BUFFER_SIZE);

	Body += string(buf, len);
	cout << len << endl;
	if (len < 0)
		throw StartLine.statusCode = 500;
	if (len < BUFFER_SIZE)
		return makeHeader();

	return READ_RESPONSE;
}

int Response::send(int clientFd)
{
	if (!Html)
		Html = new string(toHtml());
	ssize_t bufSize = Html->size();
	ssize_t	len = ::send(clientFd, Html->c_str(), bufSize, 0);
	Html->erase(0, len);
	if (len < 0)
		throw StartLine.statusCode = 500;
	if (len < bufSize)
		return END_RESPONSE;
	return SEND_RESPONSE;
}

int Response::clear()
{
	//	내부 객체 delete -> REPEAT REQUEST 반환 -> new Req로 연결(Req 삭제위치)
	delete contentResult;
	delete Html;
	return REPEAT_REQUEST;
}

#endif