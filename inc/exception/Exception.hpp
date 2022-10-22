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
// public:
// 	const char* what() const throw()
// 	{
// 		return msg.c_str();
// 	};

// 	EventLoopException(const std::string& m)
// 		: Exception("[YourExceptionName] " + m)
// 	{};

// 	~EventLoopException() throw()
//	{};
// };

class Exception
	: std::exception
{
public:
	virtual const char* what() const throw() = 0;

	Exception(const std::string& m)
	{
		msg = RED + m + END;
	};
	virtual ~Exception() throw()
	{};

protected:
	std::string msg;
};

class EventInitException
	: public Exception
{
public:
	const char* what() const throw()
	{
		return msg.c_str();
	};

	EventInitException(const std::string& m)
		: Exception("[EventInitException] " + m)
	{};
	~EventInitException() throw()
	{};
};

class EventLoopException
	: public Exception
{
public:
	const char* what() const throw()
	{
		return msg.c_str();
	};

	EventLoopException(const std::string& m)
		: Exception("[EventLoopException] " + m)
	{};

	~EventLoopException() throw()
	{};
};

#endif