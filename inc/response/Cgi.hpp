#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"

# include <unistd.h>
# include <fcntl.h>

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
		std::cout << envp[i] << std::endl;
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
		//fcntl(_fdin, F_SETFL, O_NONBLOCK);
		//	FIXME	: 임시로 waitpid
		waitpid(pid, NULL, 0);
		char buf[1024];
		size_t sz = read(_fdout, buf, 1024);
		buf[sz] = 0;
		contentsBuf = buf;
		return ;
	}
}

void 		Cgi::_post()
{
	std::ofstream ofs(url);

	code = 200;
	if (!ofs.is_open())
		code = 404;
}
void 		Cgi::_put() {};
void 		Cgi::_delete() {};

#endif  //  CGI_HPP