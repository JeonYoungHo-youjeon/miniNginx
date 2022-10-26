#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

# include <string>
# include <exception>

static const std::string RED = "\033[91m";
static const std::string END = "\033[0m";

// example

// class YourExceptionName
// 	: public Exception
// {
// 	EventLoopException(const std::string& m)
// 		: Exception("[YourExceptionName] " + m)
// 	{};
// };

class Exception
	: public std::exception
{
public:
	const char* what() const throw()
	{
		return msg.c_str();
	};

	Exception(const std::string& m)
	{
		msg = RED + m + END;
	};
	virtual ~Exception() throw() = 0;


protected:
	std::string msg;
};

Exception::~Exception() throw()
{}

class EventInitException
	: public Exception
{
public:
	EventInitException(const std::string& m)
		: Exception("[EventInitException] " + m)
	{};
};

class EventLoopException
	: public Exception
{
public:
	EventLoopException(const std::string& m)
		: Exception("[EventLoopException] " + m)
	{};
};

// class Code4xxException
// 	: public Exception
// {
// public:
// 	const std::string& what() const
// 	{
// 		return msg;
// 	};
// 	Code4xxException(const std::string& m)
// 		: Exception("[ClientException] " + m)
// 	{};
// 	virtual ~Code4xxException() {};
// };

// class Code404Exception
// 	: public Code4xxException
// {
// public:
// 	Code404Exception()
// 		: Code4xxException("404 Not Found")
// 	{};
// 	~Code404Exception() {};
// };

#endif