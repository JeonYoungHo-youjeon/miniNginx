#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"
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
: Contents(path), excutor(excutor), header(header)
{}


Cgi::Cgi(const std::string& path, const string& excutor, const vector<std::string>& params)
: Contents(path), excutor(excutor), envp(NULL)
{
	envp = new char* [params.size() + 1];
	for (size_t i = 0; i < params.size(); ++i)
	{
		envp[i] = new char[params[i].size() + 1];
		strcpy(envp[i], params[i].c_str());
	}
	envp[params.size()] = NULL;
	pwd = getcwd(0, 0);
	if (!pwd)
		return ;
}

Cgi::~Cgi()
{
	close();
	kill();
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

	if (pipe(inPipe) < 0 || pipe(outPipe))
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
	   	setenv("QUERY_STRING", header["QUERY_STRING"].c_str(), 1);				// url의 ? 뒷부분(params)

    	setenv("HTTP_COOKIE", header["COOKIE"].c_str(), 1);						// 헤더의 쿠키
    	setenv("CONTENT_LENGTH", header["CONTENT_LENGTH"].c_str(), 1);			// 헤더의 컨텐츠 길이
    	setenv("HTTP_ACCEPT", header["ACCEPT"].c_str(), 1);						// 헤더의 accept
    	setenv("HTTP_ACCEPT_LANGUAGE", header["ACCEPT_LANGUAGE"].c_str(), 1);	// 헤더의 accpet-lang
    	setenv("HTTP_USER_AGENT", header["USER_AGENT"].c_str(), 1);				// 헤더의 user-agent

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
	std::cout << "PID: " << pid << std::endl;
	if (pid)
		::kill(pid, SIGKILL);
	pid = 0;
}

#endif  //  CGI_HPP