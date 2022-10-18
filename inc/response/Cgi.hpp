#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"

struct Cgi : public Contents
{
    Cgi(const std::string& url)
    {
		ifstream	ifs(url);
    };
    ~Cgi(){};
};

#endif  //  CGI_HPP