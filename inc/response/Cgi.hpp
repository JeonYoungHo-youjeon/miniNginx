#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../define.hpp"

# include <unistd.h>
# include <fcntl.h>
# include <signal.h>

extern char** environ;

struct Cgi : public Contents
{
    Cgi(const std::string& url) : Contents(url) {};
    Cgi(const std::string& path, const string& excutor, std::map<std::string, std::string> header);
    Cgi(const std::string& path, const string& excutor, const vector<std::string>& params);

    ~Cgi();

    int         set();
    int			close();
	void		write(std::string& txt);
	std::string	read();
	void		kill();

	std::pair<int, int>	getFd()
	{
		return std::make_pair(_fdin, _fdout);
	}
	std::map<std::string, std::string> header;	
	char**		envp;
	char*		pwd;

	string		excutor;
	int 		_fdin;
	int 		_fdout;
};

Cgi::Cgi(const std::string& path, const string& excutor, std::map<std::string, std::string> header)
: Contents(path), header(header), excutor(excutor)
{}

Cgi::~Cgi()
{
	close();
}

int     Cgi::set()
{
    int inPipe[2];
	int	outPipe[2];

    char* argv[3] = {
			(char*)excutor.c_str(),
			(char*)url.c_str(),
			0
	};

	if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
		throw 500;
    
	pid = fork();

    if (pid == -1)
        throw 500;

    if (!pid)
    {
		// setenv() 함수를 이용해서 서브 프로세스의 환경변수에 params와 cgi env 를 넣어준다.
    	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    	setenv("SERVER_SOFTWARE", "miniNginx/1.0", 1);
    	setenv("CONTENT_TYPE", "Text/html", 1);

    	setenv("REMOTE_ADDR", header["REMOTE_ADDR"].c_str(), 1);				// 클라이언트의 ip
    	setenv("REQUEST_METHOD", header["REQUEST_METHOD"].c_str(), 1);			// 입력받은 메소드
    	setenv("PATH_TRANSLATED", header["PATH_TRANSLATED"].c_str(), 1);		// 파일의 절대경로
    	setenv("SCRIPT_NAME", header["SCRIPT_NAME"].c_str(), 1);				// 파일의 상대경로와 이름

		if (header["QUERY_STRING"] != "")
		   	setenv("QUERY_STRING", header["QUERY_STRING"].c_str(), 1);				// url의 ? 뒷부분(params)

		if (header.count("cookie"))
    		setenv("HTTP_COOKIE", Util::remove_crlf(header["cookie"]).c_str(), 1);						// 헤더의 쿠키
		if (header.count("content-length"))
    		setenv("CONTENT_LENGTH", Util::remove_crlf(header["content-length"]).c_str(), 1);			// 헤더의 컨텐츠 길이
		if (header.count("accept"))
    		setenv("HTTP_ACCEPT", Util::remove_crlf(header["accept"]).c_str(), 1);						// 헤더의 accept
		if (header.count("accept-language"))
	    	setenv("HTTP_ACCEPT_LANGUAGE", Util::remove_crlf(header["accept-language"]).c_str(), 1);	// 헤더의 accpet-lang	
		if (header.count("user-agent"))
 	   		setenv("HTTP_USER_AGENT", Util::remove_crlf(header["user-agent"]).c_str(), 1);				// 헤더의 user-agent
    	
		

		if (dup2(inPipe[0], 0) < 0 || dup2(outPipe[1], 1) < 0 ||
            ::close(inPipe[0]) < 0 || ::close(inPipe[1]) < 0 ||
            ::close(outPipe[0]) < 0 || ::close(outPipe[1]) < 0 ||
            execve(excutor.c_str(), argv, environ) < 0)
		{
			exit(1);
		}
    }
    else if (pid)
    {
        if (::close(inPipe[0]) < 0 || ::close(outPipe[1]) < 0)
            throw 500;
        inFd = inPipe[1];
		if (header["REQUEST_METHOD"] == "GET")
		{
			::close(inFd);
		}
		
		
        outFd = outPipe[0];
        fcntl(inFd, F_SETFL, O_NONBLOCK);
    }
	return BODY;
}

int		Cgi::close()
{
	::close(inFd);
	::close(outFd);
	return true;
}

void		Cgi::kill()
{
	if (pid)
		::kill(pid, SIGKILL);
	pid = 0;
}

#endif  //  CGI_HPP