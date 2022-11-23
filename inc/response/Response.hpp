#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <cstdio>
# include <sstream>
# include <dirent.h>
# include <sys/stat.h>

# include "Cgi.hpp"
# include "File.hpp"
# include "../http.hpp"
# include "../parse/Util.hpp"
# include "../event/Session.hpp"

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
	const Request *Req;
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
		postBody = req.bodySS.str();
		locationName = path = req.locationName;

		//	root 설정
		if (g_conf[Req->configName][locationName].is_exist("root"))
			path = g_conf[Req->configName][path]["root"][0];
		if (g_conf[Req->configName][locationName].is_exist("return"))
			return redirect(
					Util::stoi(g_conf[Req->configName][locationName]["return"][0]),
					g_conf[Req->configName][locationName]["return"][1]
					);
		//	upload
		if (g_conf[configName][locationName].is_exist("upload") && req.StartLine.method == "POST")
			path = Util::join(path, g_conf[configName][locationName]["upload"][0], '/');
		fileName = req.fileName;

		path = Util::join(path, fileName, '/');
		ext = req.ext;

		progress = READY;
		return statement = execute();
	}

	int redirect(int code, const string& location);

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
		it = Header.find("Content-Type");
		if (it == Header.end())
			Header["Content-Type"] = g_conf.getContentType(ext);
		Header["Content-Length"] = Util::to_string(Body.size());
		{    /* 필요 헤더*/    }
		std::cerr << Header["Content-Type"] << std::endl;
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


//TODO : 슬래시 여부에 따른 분기 구현
//디렉토리일때 슬래쉬 유무 확인해서 슬래쉬 있을때만 index/autoindex 바로 보여주고,
//슬래쉬 없는데 디렉토리면 301 + 헤더에 location 첨부.
//해당 디렉토리가 없을때 404
//해당 디렉토리가 있으나 index/autoindex가 꺼져있으면 403
//엔진엑스에서는 autoindex 와 index가 같이 있으면 index만 동작함



	string get_dirlist_page(string path, string head)
	{
		string ret;
		string page;

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(path.c_str()))) 
		{
			while ((ent = readdir(dir))) 
			{
				struct stat statbuf;
				std::string tmp = ent->d_name;
				std::string checker = path + tmp;
				stat(checker.c_str(), &statbuf);
				if (S_ISDIR(statbuf.st_mode))
					tmp += "/" ;

				page += "<a href=\"";
				page += tmp;
				page += "\">";
				page += tmp;
				page += "</a>\n" ;
			}
			closedir (dir);
		} 
	
		ret =
			"<html>\n"
			"<head><title>Index of " + head + " </title></head>\n"
			"<body>\n"
			"<h1>Index of " + head + " </h1><hr><pre>\n" +
			page +
			"</pre><hr></body>\n"
			"</html>\n";
			
		return ret;
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
		if (g_conf[configName][locationName].is_exist("return"))
			return redirect(
					StartLine.statusCode = Util::stoi(g_conf[configName][locationName]["return"][0]),
					g_conf[configName][locationName]["return"][1]);
		if (Req->StartLine.method != "GET" && Req->StartLine.method != "POST")
			throw 400;
		
		if (g_conf[Req->configName][Req->locationName].is_exist(ext))
		{
			Session *session = Req->session;
			std::map<string, string> cookies = Req->cookies;

			if (Req->cookies.count(ext) == 0)
			{
				Header["set-cookie"] = ext + "=" + session->set("") + ";";
			}
			else
			{
				string tmp = session->get(cookies[ext]);

				if (!tmp.empty())
				{
					tmp.insert(0, "COOKIE=");
					tmp += ";";
					params.push_back(tmp);
				}
			}
			if (Req->StartLine.method == "POST")
				session->Session[cookies[ext]] = Req->bodySS.str();
	
			Header["Content-Type"] = "Text/html";
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
	std::cout << "=========[RESPONSE WRTIE]==========" << std::endl;
	ssize_t	len = ::write(contentResult->inFd, postBody.c_str(), postBody.size());
	std::cout << "len : " << len << std::endl;

	if (len < 0)
		throw StartLine.statusCode = 500;

	if (len == postBody.size())
	{
		postBody.erase(0, len);
		return makeHeader();
	}

	postBody.erase(0, len);
	//	전부 보냈으면 결과 값 받아오기 위해 READ_RESPONSE 반환
	if (len < postBody.size())
		return READ_RESPONSE;

	return WRITE_RESPONSE;
}

int 	Response::read()
{
	//	읽고 읽을것이 남아있으면 READ_RESPONSE 반환
	char buf[BUFFER_SIZE];
	size_t	len = ::read(contentResult->outFd, buf, BUFFER_SIZE);

	Body += string(buf, len);
	//	< BUFFER_SIZE 밑에 있어서 닿을 수 없던 부분 수정
	if (len < 0)
		throw StartLine.statusCode = 500;
	if (len < BUFFER_SIZE)
		return makeHeader();

	return READ_RESPONSE;
}

int Response::redirect(int code, const string& location)
{
	Header["Location"] = location;
	StartLine.statusCode = code;
	StartLine.reasonPhrase = g_conf.getStatusMsg(code);
	return makeHeader();
}


int Response::send(int clientFd)
{
	if (!Html)
		Html = new string(toHtml());

	ssize_t bufSize = Html->size();
	ssize_t	len = ::send(clientFd, Html->c_str(), bufSize, 0);
	std::cout << "\t==========[SEND SIZE]==========" << std::endl;
	std::cout << "len : " << len << ", bufSize : " << bufSize << std::endl;

	if (len > 0)
		Html->erase(0, len);

	if (len < 0)
		throw StartLine.statusCode = 500;
	if (Html->empty())
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