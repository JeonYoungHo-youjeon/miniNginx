#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
# include "Cgi.hpp"
# include "File.hpp"
# include "../parse/Config.hpp"
# include "../http.hpp"
# include "../parse/Util.hpp"

/*
 *  Http Test
 */
# include "../http.hpp"

class ResponseImpl : public Response
{
public:
    ResponseImpl(const Config& config) {}
    ResponseImpl(const Request& req, const Config& conf)
    {
		/*
		 * IP:port
		 * 		  /resources/path
		 * 		  				 .ext
		 * 							 ?k1=v1 & k2=v2
		 */
		std::string url = req.url;
		std::string	host = "*:8000";	//	서버 요청 파싱 필요
		if (url.empty())
			throw UrlException();

		std::string path;
		std::vector<std::string> params;

		{
			std::vector<std::string> urlToken(Util::split(url, '?'));
			if (urlToken.size() > 2)
				throw UrlException();
			path = urlToken.front();
			if (urlToken.size() == 2)
				params = Util::split(urlToken.back(), '&');
		}

		std::pair<std::string, std::string>	divpath = Util::divider(path, '/');
		std::string	location = divpath.first;
		std::string	resource = divpath.second;
		//std::string	ext;

		while (location != "")
		{
			std::cout << "[" << location << "]" <<  std::endl;
			std::cout << "[" << resource << "]" << std::endl;
			std::cout << std::endl;
			try
			{
				std::string goTo = conf[host][location]["root"][0] + resource;
				std::string ext(resource.begin() + resource.rfind('.'), resource.end());
				std::string executer(conf[host][location][ext][0]);
				std::cout << "==== Find Right Path ===" << std::endl;
				std::cout << "Root + Resource's Path : " << goTo << std::endl;
				std::cout << "CGI Executer : " << executer << std::endl;
				std::cout << "=========== = ==========" << std::endl;
			} catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}

			divpath = Util::divider(divpath, '/');
			location = divpath.first;
			resource = divpath.second;
		}
		std::cout << "[" << location << "]" <<  std::endl;
		std::cout << "[" << resource << "]" << std::endl;
		std::cout << std::endl;
		std::string executer;
		/*
		try
		{
			executer = conf[host][]
		}
		catch ()
		{

		}
		 */
		/*
        if (true)
			mContents = new File(req.url);
        else if (true)
            mContents = new Cgi(req.url);
		*/
	}
    ~ResponseImpl(){
        //if (mContents != nullptr)
		//	delete mContents;
    }
    std::string str()
    {
		return mContents->getContents();
    }
protected:
private:
public:
	class UrlException : public std::exception {
	public:
		const char* what() const throw();
	};

protected:
private:
     Contents*    mContents;	//	Body of Response
};

const char* ResponseImpl::UrlException::what() const throw()
{
	return "Url Parsing Error";
}

#endif