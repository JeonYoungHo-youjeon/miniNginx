#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"

# include <unistd.h>
# include <fcntl.h>
# include <signal.h>

const int BUFFER_SIZE = 1024;

struct Cgi : public Contents
{
    Cgi(const std::string& path, const std::string& filename, const string& body, const vector<std::string>& params, const string& excutor);
    ~Cgi();

	void child()
	{}
	void parent()
	{}
	void		_get();
	void		_post();
	void		_put();
	void		_delete();
	std::string execute();

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

Cgi::Cgi(const std::string& path, const std::string& filename, const string& body, const vector<std::string>& params, const string& excutor)
: Contents(path + filename, body), excutor(excutor), envp(NULL)
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
}

void 	Cgi::_get()
{
	int inFd[2];
	int	outFd[2];

	if (pipe(inFd) < 0 || pipe(outFd))
	{
		code = 500;
		return ;
	}
	char* argv[3] = {
			(char*)excutor.c_str(),
			(char*)url.c_str(),
			0
	};
	pid_t	pid = fork();

	if (pid == -1)
	{
		code = 500;
		return ;    //	X
	}
	else if (!pid)
	{
		if (dup2(inFd[0], 0) < 0)
			exit(1);
		if (dup2(outFd[1], 1) < 0)
			exit(1);
		if (close(inFd[0]) < 0)
			exit(1);
		if (close(inFd[1]) < 0)
			exit(1);
		if (close(outFd[0]) < 0)
			exit(1);
		if (close(outFd[1]) < 0)
			exit(1);
		if (execve(excutor.c_str(), argv, envp) < 0)
			exit(1);
	}
	else
	{
		setPid(pid);
		close(inFd[0]);
		close(outFd[1]);
		_fdin = inFd[1];
		_fdout = outFd[0];
		fcntl(_fdin, F_SETFL, O_NONBLOCK);
		//	FIXME	: 임시로 waitpid
		//waitpid(pid, NULL, 0);
		//contentsBuf = this->read();
		return ;
	}
	close(outFd[0]);
	close(outFd[1]);
	close(inFd[0]);
	close(inFd[1]);
}

void 		Cgi::_post()
{
	int inFd[2];
	int	outFd[2];

	if (pipe(inFd) < 0 || pipe(outFd))
	{
		code = 500;
		return ;
	}
	char* argv[3] = {
			(char*)excutor.c_str(),
			(char*)url.c_str(),
			0
	};
	pid_t	pid = fork();

	if (pid == -1)
	{
		code = 500;
		return ;    //	X
	}
	else if (!pid)
	{
		if (dup2(inFd[0], 0) < 0)
			exit(1);
		if (dup2(outFd[1], 1) < 0)
			exit(1);
		if (close(inFd[0]) < 0)
			exit(1);
		if (close(inFd[1]) < 0)
			exit(1);
		if (close(outFd[0]) < 0)
			exit(1);
		if (close(outFd[1]) < 0)
			exit(1);
		if (execve(excutor.c_str(), argv, envp) < 0)
			exit(1);
	}
	else
	{
		setPid(pid);
		close(inFd[0]);
		close(outFd[1]);
		_fdin = inFd[1];
		_fdout = outFd[0];
		this->write(contentsBuf);
		fcntl(_fdin, F_SETFL, O_NONBLOCK);
		//	FIXME	: 임시로 waitpid
		//waitpid(pid, NULL, 0);
		contentsBuf = this->read();
		return ;
	}
	close(outFd[0]);
	close(outFd[1]);
	close(inFd[0]);
	close(inFd[1]);
	//this->_get();
	//this->write(contentsBuf);
	//this->read();
}

void		Cgi::write(std::string& txt)
{
	size_t	len = ::write(_fdin, txt.c_str(), txt.size());
	txt.erase(0, len);
}
std::string	Cgi::read()
{
	char	buf[BUFFER_SIZE];
	size_t	len = ::read(_fdout, buf, BUFFER_SIZE);
	buf[len] = 0;
	return std::string(buf);	//	read_size = $(return).size();
}

void		Cgi::kill()
{
	::kill(pid, SIGKILL);
}

void 		Cgi::_put() {};
void 		Cgi::_delete() {};

#endif  //  CGI_HPP