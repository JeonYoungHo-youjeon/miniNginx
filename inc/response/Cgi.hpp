#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"
# include "../define.hpp"

# include <unistd.h>
# include <fcntl.h>
# include <signal.h>

struct Cgi : public Contents
{
    Cgi(const std::string& url) : Contents(url) {};
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

	char**		envp;
	string		excutor;
	int 		_fdin;
	int 		_fdout;
};

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
}
Cgi::~Cgi()
{
	if (envp)
	{
		for (size_t i = 0; envp[i] != NULL; ++i)
			delete[] envp[i];
		delete[] envp;
	}
	::close(inFd);
	::close(outFd);
	::kill(pid, SIGKILL);
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
		if (dup2(inPipe[0], 0) < 0 || dup2(outPipe[1], 1) < 0 ||
            ::close(inPipe[0]) < 0 || ::close(inPipe[1]) < 0 ||
            ::close(outPipe[0]) < 0 || ::close(outPipe[1]) < 0 ||
            execve(excutor.c_str(), argv, envp) < 0)
			exit(1);
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
	::kill(pid, SIGKILL);
}

#endif  //  CGI_HPP