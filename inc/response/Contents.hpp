#ifndef CONTENTS_HPP
# define CONTENTS_HPP

# include <iostream>
# include <fstream>
# include <cstdio>

struct Contents
{
	//	Methods
    //Contents(const std::string& url)
	//: mUrl(url) {};
	Contents(const std::string& url, const std::string& body = "")
	: mUrl(url), mContents(body) {};
    virtual ~Contents(){};

    virtual std::string	_get() = 0;
	virtual void		_post() = 0;
	virtual void		_put() = 0;
	virtual void		_delete() =0;

	//	Variables
	std::string	mContents;
	std::string mUrl;
	int 		mCode;
};      //  Contents

#endif  //  CONTENTS_HPP