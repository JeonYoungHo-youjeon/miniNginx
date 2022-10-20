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
	: mContents(0)
    {
		if (req.url.empty())
			throw UrlException();

		mHost = "*:8000";	//	서버 요청 파싱 필요?
		parse_query(req.url);

		std::pair<std::string, std::string>	divpath = Util::divider(mPath, '/');

		std::string goTo;		//	Location
		std::string ext;		//	확장자
		std::string executor;	//	실행 바이너리

		while (divpath.first != "" || conf[mHost].is_exist(divpath.first))
		{
			try
			{
				goTo = conf[mHost][divpath.first]["root"][0] + divpath.second;
				ext = std::string(divpath.second.begin() + divpath.second.rfind('.'), divpath.second.end());
				executor = std::string(conf[mHost][divpath.first][ext][0]);

				std::cout << "==== Find Right Path ===" << std::endl;
				std::cout << "Root + Resource's Path : " << goTo << std::endl;
				std::cout << "CGI Executer : " << executor << std::endl;
				std::cout << "=========== = ==========" << std::endl;
				break ;
			} catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
			divpath = Util::divider(divpath, '/');
		}

		//	최종 Location과 root_dir이 합쳐진 root_dir + 파일 경로 존재
		std::cout << std::endl;
		std::cout << "location : " << divpath.first << std::endl;
		std::cout << "resource : " << divpath.second << std::endl;

		if (executor != "")
		{
			mContents = new Cgi(goTo, executor);
		}
		else
		{
			mContents = new File(goTo);
		}
	}
    ~ResponseImpl(){
        if (mContents != nullptr)
			delete mContents;
    }
    std::string str()
    {
		return mContents->getContents();
    }
protected:
private:
	void cgi(const std::pair<std::string, std::string>& path);
	void file(const std::pair<std::string, std::string>& path);
	void parse_query(const std::string& url);
public:
	class UrlException : public std::exception {
	public:
		const char* what() const throw();
	};

protected:
private:
     Contents*    				mContents;	//	Body of Response
	 std::vector<std::string>	mParams;
	 std::string 				mPath;
	 std::string 				mHost;	//	임시 변수
};

const char* ResponseImpl::UrlException::what() const throw()
{
	return "Url Parsing Error";
}
void ResponseImpl::cgi(const std::pair<std::string, std::string>& path)
{

}
void ResponseImpl::file(const std::pair<std::string, std::string>& path)
{

}
void ResponseImpl::parse_query(const std::string& url)
{
	std::vector<std::string> urlToken(Util::split(url, '?'));
	if (urlToken.size() > 2)
		throw UrlException();
	mPath = urlToken.front();
	if (urlToken.size() == 2)
		mParams = Util::split(urlToken.back(), '&');
}

#endif