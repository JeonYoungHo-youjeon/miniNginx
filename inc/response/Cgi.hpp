#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"

struct Cgi : public Contents
{
    Cgi(const std::string& url, const Config& conf)
    {
		std::ifstream	ifs(url);
    };
    ~Cgi(){};
};

#endif  //  CGI_HPP