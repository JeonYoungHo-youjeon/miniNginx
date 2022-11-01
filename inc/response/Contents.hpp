#ifndef CONTENTS_HPP
# define CONTENTS_HPP

# include <iostream>
# include <fstream>
# include <cstdio>

struct Contents
{
	Contents(const std::string& url, const std::string& body = "")
	: url(url), contentsBuf(body) {};
    virtual ~Contents(){};

    virtual void		_get() = 0;
	virtual void		_post() = 0;
	virtual void		_put() = 0;
	virtual void		_delete() =0;

	const int 			getPid() const ;
	void 				setPid(int pid);

	const std::string&	getBody() const ;

	//	Variables
	std::string	contentsBuf;
	std::string url;
	int 		code;
	int 		pid;
};      //  Contents

const int	Contents::getPid() const
{
	return pid;
}
void 		Contents::setPid(int pid)
{
	this->pid = pid;
}
const std::string&	Contents::getBody() const
{
	return contentsBuf;
}
#endif  //  CONTENTS_HPP