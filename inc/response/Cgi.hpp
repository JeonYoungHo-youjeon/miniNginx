#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"

# include <unistd.h>

struct Cgi : public Contents
{
    Cgi(const std::string& url, const std::string& body, const string& ext, const vector<string>& params);
    ~Cgi();

	void child()
	{}
	void parent()
	{}
	std::string	_get();
	void		_post();
	void		_put();
	void		_delete();
	std::string execute();

	char**		mEnv;
	string		mExcutor;
};

Cgi::Cgi(const std::string& url, const std::string& body, const string& excutor, const vector<string>& params)
: Contents(url, body), mExcutor(excutor), mEnv(NULL)
{
	mEnv = new char* [params.size() + 1];
	for (size_t i = 0; i < params.size(); ++i)
	{
		mEnv[i] = new char[params[i].size() + 1];
		strcpy(mEnv[i], params[i].c_str());
	}
	mEnv[params.size()] = NULL;
}
Cgi::~Cgi()
{
	if (mEnv)
	{
		for (size_t i = 0; mEnv[i] != NULL; ++i)
			delete[] mEnv[i];
		delete[] mEnv;
	}
}

std::string Cgi::_get()
{
	mCode = 200;

	int fd[2];
	if (pipe(fd) < 0)
	{
		mCode = 500;
		return "Internal Server Error";	//	X
	}
	pid_t	pid = fork();

	if (pid == -1)
	{
		mCode = 500;
		return "Internal Server Error";    //	X
	}
	else if (pid)
	{
		write(fd[0], mContents.c_str(), mContents.size() + 1);
		execve(mExcutor, mUrl, mEnv);
	}
	else
	{
		//	Child
	}
	return mContents;
}

void 		Cgi::_post()
{
	std::ofstream ofs(mUrl);

	mCode = 200;
	if (!ofs.is_open())
		mCode = 404;
		//throw Code404Exception();
}
void 		Cgi::_put() {};
void 		Cgi::_delete() {};

#endif  //  CGI_HPP