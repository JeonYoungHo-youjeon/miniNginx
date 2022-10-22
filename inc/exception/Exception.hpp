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

#endif