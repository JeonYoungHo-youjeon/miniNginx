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

		std::string goTo;		//	Location
		std::string ext;		//	확장자
		std::string executer;	//	실행 바이너리

		while (divpath.first != "")
		{
			try
			{
				/*
				 * 최대한 디테일한 PATH를 찾고 만약 찾았다면 No Exception
				 * 못 찾았다면 상위 PATH로 이동
				 * /a/b/c/d.sh가 있다면
				 * /a/b/c에서 d.sh 실행 시도
				 * ->	/a/b에서 /c/d.sh 실행 시도
				 * ->	->	/a에서 /b/c/d.sh 실행 시도
				 * ->	->	->	/에서 /a/b/c/d.sh 실행 시도
				 */
				goTo = conf[host][divpath.first]["root"][0] + divpath.second;
				ext = std::string(divpath.second.begin() + divpath.second.rfind('.'), divpath.second.end());
				executer = std::string(conf[host][divpath.first][ext][0]);

				std::cout << "==== Find Right Path ===" << std::endl;
				std::cout << "Root + Resource's Path : " << goTo << std::endl;
				std::cout << "CGI Executer : " << executer << std::endl;
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