#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"

struct Cgi : public Contents
{
    Cgi(const std::string& url, const std::string& executor)
    {
		std::ifstream	ifs(url);
    };
    ~Cgi(){};

	void child()
	{}
	void parent()
	{}
	//std::pair<std::string, std::string> div
};

#endif  //  CGI_HPP