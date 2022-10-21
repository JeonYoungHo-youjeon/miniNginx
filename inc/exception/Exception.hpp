#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

# include <string>

static const std::string RED = "\033[91m";
static const std::string END = "\033[0m";

// example

// class YourExceptionName
// 	: public Exception
// {
// public:
// 	const std::string& what() const
// 	{
// 		return msg;
// 	};

// 	EventLoopException(const std::string& m)
// 		: Exception("[YourExceptionName] " + m)
// 	{};

// 	~EventLoopException() {};

// };

class Exception
{
public:
	virtual const std::string& what() const = 0;

	Exception(const std::string& m)
	{
		msg = RED + m + END;
	};
	virtual ~Exception() {};

protected:
	std::string msg;
};

class EventInitException
	: public Exception
{
public:
	const std::string& what() const
	{
		return msg;
	};

	EventInitException(const std::string& m)
		: Exception("[EventInitException] " + m)
	{};
	~EventInitException() {};

};

class EventLoopException
	: public Exception
{
public:
	const std::string& what() const
	{
		return msg;
	};

	EventLoopException(const std::string& m)
		: Exception("[EventLoopException] " + m)
	{};

	~EventLoopException() {};

};

#endif