#ifndef CONTENTS_HPP
# define CONTENTS_HPP

# include <iostream>
# include <fstream>
# include <cstdio>

struct Contents
{
	Contents(const std::string& url)
	: url(url), pid(0) {};
    virtual ~Contents(){};

	const int 			getPid() const ;
	void 				setPid(int pid);
	virtual int			set() = 0;
	virtual int			close() = 0;
	virtual void		kill() = 0;

	//	Variables
	std::string url;
	int			inFd;
	int			outFd;
	pid_t 		pid;
};      //  Contents

const int	Contents::getPid() const
{
	return pid;
}
void 		Contents::setPid(int pid)
{
	this->pid = pid;
}
#endif  //  CONTENTS_HPP