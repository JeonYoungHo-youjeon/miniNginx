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
		std::string::size_type pos = mPath.rfind('.');
		if (pos != static_cast<std::string::size_type>(-1))
			mExt = std::string(mPath.begin() + pos, mPath.end());

		std::pair<std::string, std::string>	divpath = Util::divider(mPath, '/');

		while (divpath.first != "" && !conf[mHost].is_exist(divpath.first))
			divpath = Util::divider(divpath, '/');
		if (divpath.first.empty())
			divpath.first = "/";
		mLocation = divpath.first;
		mResource = divpath.second;
		std::cout << "Ext : " << mExt<< std::endl;
		std::cout << "location : " << mLocation << std::endl;
		std::cout << "resource : " << mResource << std::endl;
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
	 std::vector<std::string>	mParams;	//	parameter
	 std::string 				mPath;		//	URL without query
	 std::string 				mLocation;	//	LocationAccessor
	 std::string 				mResource;	//	ResourceName
	 std::string 				mExt;		//	Extension
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