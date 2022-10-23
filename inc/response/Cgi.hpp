#ifndef CGI_HPP
# define CGI_HPP

# include "Contents.hpp"
# include "../parse/Config.hpp"
# include "../exception/Exception.hpp"

struct Cgi : public Contents
{
    Cgi(const std::string& url, const std::string& body)
    : Contents(url, body) {};
    ~Cgi(){};

	void child()
	{}
	void parent()
	{}
	//std::pair<std::string, std::string> div
	std::string	_get();
	void		_post();
	void		_put();
	void		_delete();
};

std::string Cgi::_get()
{
	std::ifstream ifs(mUrl);

	//	TODO : 404 맞는지 확인해보기
	if (!ifs.is_open())
		throw Code404Exception();
	std::string buf;
	while (getline(ifs, buf, '\n'))
		mContents += buf;
	return mContents;
}

void 		Cgi::_post()
{
	std::ofstream ofs(mUrl);

	if (!ofs.is_open())
		throw Code404Exception();

}
void 		Cgi::_put() {};
void 		Cgi::_delete() {};

#endif  //  CGI_HPP