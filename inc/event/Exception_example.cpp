#include <string>
#include <iostream>

class Exception
{
};

class ParsingException: public Exception
{
};

class EventInitException: public Exception
{
};

class EventLoopException: public Exception
{
};


int main()
{
	Exception e;

	
}