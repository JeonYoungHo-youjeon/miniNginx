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

	int 			getPid() const ;
	void 				setPid(int pid);
	virtual int			set() = 0;
	virtual int			close() = 0;
	virtual void		kill() = 0;
	virtual bool 		checkNull(){ return true ;}

	//	Variables
	std::string url;
	int			inFd;
	int			outFd;
	pid_t 		pid;
};      //  Contents

int	Contents::getPid() const
{
	return pid;
}
void 		Contents::setPid(int pid)
{
	this->pid = pid;
}
#endif  //  CONTENTS_HPP