#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
# include "Cgi.hpp"
# include "File.hpp"
# include "../parse/Config.hpp"
//# include "../parse/Config.hpp"

/*
 *  Http Test
 */
# include "../http.hpp"

class ResponseImpl
{
public:
    ResponseImpl(const int errorCode) {}
    ResponseImpl(const Config& config) {}
    ResponseImpl(const Request& req)
    {
        if (/* req.url needs File */true)
            mContents = new File(req.url);
        else if (/* req.url needs CGI */ true)
            mContents = new Cgi(req.url);
    }
    ~ResponseImpl(){
        if (mContents)
            delete mContents;
    }
    std::string str()
    {
        return mContents->getContents();
    }
protected:
private:
public:
protected:
private:
     Contents*    mContents;
};

#endif