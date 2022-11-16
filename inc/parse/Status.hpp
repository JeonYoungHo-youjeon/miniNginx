#ifndef STATUS_HPP
#define STATUS_HPP

#include <map>
#include <string>

class StatusMsg : public std::map<int, std::string>
{
private:
	StatusMsg(const StatusMsg&);
	StatusMsg& operator=(const StatusMsg&);
public:
	StatusMsg();
	~StatusMsg();
};
StatusMsg::~StatusMsg(){};
StatusMsg::StatusMsg()
{
	(*this)[100] = "Continue";
	(*this)[101] = "Switching Protocols";

	(*this)[200] = "OK";
	(*this)[201] = "Created";
	(*this)[202] = "Accepted";
	(*this)[203] = "Non-Authoritative Information";
	(*this)[204] = "No Content";
	(*this)[205] = "Reset Content";
	(*this)[206] = "Partial Content";

	(*this)[300] = "Multiple Choices";
	(*this)[301] = "Moved Permanently";
	(*this)[302] = "Found";
	(*this)[303] = "See Other";
	(*this)[304] = "Not Modified";
	(*this)[305] = "Use Proxy";
	(*this)[307] = "Temporary Redirect";

	(*this)[400] = "Bad Request";
	(*this)[401] = "Unauthorized";
	(*this)[402] = "Payment Required";
	(*this)[403] = "Forbidden";
	(*this)[404] = "Not Found";
	(*this)[405] = "Method Not Allowed";
	(*this)[406] = "Not Acceptable";
	(*this)[407] = "Proxy Authentication Required";
	(*this)[408] = "Request Timeout";
	(*this)[409] = "Conflict";
	(*this)[410] = "Gone";
	(*this)[411] = "Length Required";
	(*this)[412] = "Precondition Failed";
	(*this)[413] = "Payload Too Large";
	(*this)[414] = "URI Too Long";
	(*this)[415] = "Unsupported Media Type";
	(*this)[416] = "Range Not Satisfiable";
	(*this)[417] = "Expectation Failed";
	(*this)[426] = "Upgrade Required";

	(*this)[500] = "Internal Server Error";
	(*this)[501] = "Not Implemented";
	(*this)[502] = "Bad Gateway";
	(*this)[503] = "Service Unavailable";
	(*this)[504] = "Gateway Timeout";
	(*this)[505] = "HTTP Version Not Supported";
}

#endif //STATUS_HPP
